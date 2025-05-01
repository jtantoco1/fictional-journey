self_test_src = files(
    './string_metric.test.cpp',
    './wordlist_base.test.cpp',
)

tests_exe = executable(
    'smartmessage_unit_tests',
    self_test_src,
    dependencies: [catch2_dep],
    include_directories: ['../src'],
    link_with: [main_lib],
)

test('smartmessage_unit_tests', tests_exe)