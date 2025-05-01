#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <sstream>

// ImGui includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include <GLFW/glfw3.h>

// Project includes
#include "src/wordlist.h"

// Dictionary path
const std::string DICT_PATH = "dict_en/google_10k";

// Global wordlist managers
std::unique_ptr<SmartMessage::PrefixTree> prefixTree;
std::unique_ptr<SmartMessage::BKTree> bkTree;

// Input text buffer and current word
std::string inputText = "";
std::string currentWord = "";
int cursorPos = 0;
bool showTooltip = false;

// Results
std::vector<std::string> autoCompleteResults;
std::vector<std::pair<std::string, int>> spellCheckResults;

// Function to find current word at cursor position
std::string getCurrentWord(const std::string& text, int cursorPos) {
    // If text is empty or cursor is at position 0, return empty string
    if (text.empty() || cursorPos <= 0) {
        return "";
    }

    // Find the start of the word
    int start = cursorPos - 1;
    while (start >= 0 && SmartMessage::WordParser::is_word_char(text[start])) {
        start--;
    }
    start++;

    // Find the end of the word
    int end = cursorPos - 1;
    while (end < static_cast<int>(text.length()) && SmartMessage::WordParser::is_word_char(text[end])) {
        end++;
    }

    // Extract the word
    if (start <= end && start < static_cast<int>(text.length())) {
        return text.substr(start, end - start + 1);
    }

    return "";
}

// Function to highlight misspelled words
void renderText(const std::string& text) {
    ImGui::BeginChild("TextHighlight", ImVec2(ImGui::GetWindowWidth() - 20, 200), true);
    
    SmartMessage::WordsIterator words(text);
    std::stringstream ss(text);
    std::string word;
    std::string remainingText = text;
    
    for (auto it = words.begin(); it != words.end(); ++it) {
        std::string currentWord = *it;
        
        // Find position of the word in the remaining text
        size_t pos = remainingText.find(currentWord);
        if (pos != std::string::npos) {
            // Output any text before the word
            ImGui::TextUnformatted(remainingText.substr(0, pos).c_str());
            ImGui::SameLine(0.0f, 0.0f);
            
            // Check if word is misspelled (not in dictionary)
            std::string normalizedWord = SmartMessage::WordParser::normalize_word(currentWord);
            auto closeMatches = bkTree->find_closest(normalizedWord, 0);
            bool isMisspelled = closeMatches.empty();
            
            // Highlight misspelled words in red
            if (isMisspelled && !normalizedWord.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                ImGui::TextUnformatted(currentWord.c_str());
                ImGui::PopStyleColor();
            } else {
                ImGui::TextUnformatted(currentWord.c_str());
            }
            ImGui::SameLine(0.0f, 0.0f);
            
            // Update remaining text
            remainingText = remainingText.substr(pos + currentWord.length());
        }
    }
    
    // Output any remaining text
    if (!remainingText.empty()) {
        ImGui::TextUnformatted(remainingText.c_str());
    }
    
    ImGui::EndChild();
}

// Function to update word suggestions based on current cursor position
void updateSuggestions() {
    currentWord = getCurrentWord(inputText, cursorPos);
    
    if (!currentWord.empty()) {
        // Update autocomplete suggestions
        autoCompleteResults = prefixTree->find_partial(currentWord);
        
        // Update spell check suggestions if word is complete
        bool isWordComplete = cursorPos < static_cast<int>(inputText.length()) && 
                             !SmartMessage::WordParser::is_word_char(inputText[cursorPos]);
        
        if (isWordComplete) {
            // Check if the word exists in dictionary
            auto exactMatches = bkTree->find_closest(currentWord, 0);
            if (exactMatches.empty()) {
                // Word not in dictionary, get close matches
                spellCheckResults = bkTree->find_closest(currentWord, 3);
            } else {
                spellCheckResults.clear();
            }
        }
    } else {
        autoCompleteResults.clear();
        spellCheckResults.clear();
    }
}

// Function to add a word to the dictionary
void addWordToDictionary(const std::string& word) {
    if (!word.empty()) {
        std::string normalizedWord = SmartMessage::WordParser::normalize_word(word);
        if (!normalizedWord.empty()) {
            prefixTree->add(normalizedWord);
            bkTree->add(normalizedWord);
            
            // Save to appropriate dictionary file
            if (!normalizedWord.empty()) {
                char firstLetter = normalizedWord[0];
                std::string filename = DICT_PATH + "/" + firstLetter + ".smwdl";
                
                // Create directories if they don't exist
                std::filesystem::create_directories(DICT_PATH);
                
                // Open file in append mode
                std::ofstream file(filename, std::ios::app);
                if (file.is_open()) {
                    file << normalizedWord << std::endl;
                    file.close();
                }
            }
        }
    }
}

// GLFW error callback
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    // Initialize wordlist managers
    prefixTree = std::make_unique<SmartMessage::PrefixTree>();
    bkTree = std::make_unique<SmartMessage::BKTree>();
    
    // Load dictionaries
    prefixTree->load_dictionaries(DICT_PATH);
    bkTree->load_dictionaries(DICT_PATH);
    
    // Setup GLFW window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
        
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(800, 600, "SmartMessage", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Create main window
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("SmartMessage", nullptr, ImGuiWindowFlags_NoCollapse);
        
        // Display title
        ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "SmartMessage - Text Editor with Autocomplete and Spell Checking");
        ImGui::Separator();
        
        // Text input area
        ImGui::Text("Editor:");
        
        // Capture cursor position before input
        static int lastCursorPos = 0;
        
        // Input text widget
        ImGui::InputTextMultiline("##TextInput", &inputText, ImVec2(-1, 200));
        
        // Get cursor position after input
        cursorPos = ImGui::GetIO().MousePos.x > 0 ? ImGui::GetInputTextState()->StbState.cursor : lastCursorPos;
        lastCursorPos = cursorPos;
        
        // Update suggestions based on cursor position
        updateSuggestions();
        
        // Show text with highlighted misspelled words
        ImGui::Text("Text with highlighted misspelled words:");
        renderText(inputText);
        
        // Show autocomplete tooltip
        if (!currentWord.empty() && !autoCompleteResults.empty() && showTooltip) {
            ImGui::BeginTooltip();
            ImGui::Text("Autocomplete Suggestions:");
            for (const auto& suggestion : autoCompleteResults) {
                if (ImGui::Selectable(suggestion.c_str())) {
                    // Replace current word with suggestion
                    size_t wordStart = cursorPos - currentWord.length();
                    inputText.replace(wordStart, currentWord.length(), suggestion);
                    cursorPos = wordStart + suggestion.length();
                    currentWord = suggestion;
                }
            }
            ImGui::EndTooltip();
        }
        
        // Spell check suggestions sidebar
        ImGui::SameLine();
        ImGui::BeginChild("Suggestions", ImVec2(200, 200), true);
        ImGui::Text("Spell Check Suggestions:");
        
        if (spellCheckResults.empty()) {
            ImGui::TextDisabled("No suggestions");
        } else {
            for (const auto& [suggestion, distance] : spellCheckResults) {
                if (ImGui::Selectable(suggestion.c_str())) {
                    // Find the position of the current word in text
                    SmartMessage::WordsIterator words(inputText);
                    int wordIndex = 0;
                    std::string wordToReplace;
                    
                    for (auto it = words.begin(); it != words.end(); ++it) {
                        if (SmartMessage::WordParser::normalize_word(*it) == SmartMessage::WordParser::normalize_word(currentWord)) {
                            wordToReplace = *it;
                            break;
                        }
                        wordIndex++;
                    }
                    
                    if (!wordToReplace.empty()) {
                        size_t pos = inputText.find(wordToReplace);
                        if (pos != std::string::npos) {
                            inputText.replace(pos, wordToReplace.length(), suggestion);
                        }
                    }
                }
            }
        }
        ImGui::EndChild();
        
        // Dictionary management
        ImGui::Separator();
        ImGui::Text("Dictionary Management:");
        
        // Add word to dictionary
        static std::string newWord;
        ImGui::InputText("New Word", &newWord);
        ImGui::SameLine();
        if (ImGui::Button("Add Word")) {
            addWordToDictionary(newWord);
            newWord.clear();
        }
        
        // Add current word to dictionary
        if (!currentWord.empty()) {
            ImGui::SameLine();
            if (ImGui::Button("Add Current Word")) {
                addWordToDictionary(currentWord);
                // Refresh suggestions
                updateSuggestions();
            }
        }
        
        // Add all words to dictionary
        if (ImGui::Button("Add All Words to Dictionary")) {
            SmartMessage::WordsIterator words(inputText);
            for (auto it = words.begin(); it != words.end(); ++it) {
                addWordToDictionary(*it);
            }
            // Refresh suggestions
            updateSuggestions();
        }
        
        // Toggle autocomplete tooltip
        ImGui::Checkbox("Show Autocomplete Tooltip", &showTooltip);
        
        ImGui::End();
        
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}