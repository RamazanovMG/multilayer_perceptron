## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ##
## @copyright Copyright [2023] <Phylis Cristobal>  ##
## @file Makefile                                  ##
## @date 2023-08-19                                ##
## @author Phylis Cristobal (phyliscr)             ##
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ##
##                   COLOR THEME                   ##
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ##
DEFAULT=\033[0m 
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ##
WHITE_FG=\033[37m 
RED_FG=\033[31m 
GREEN_FG=\033[32m 
YELLOW_FG=\033[33m 
BLUE_FG=\033[34m 
PURPLE_FG=\033[35m 
CYAN_FG=\033[36m 
BLACK_FG=\033[30m 
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ##
WHITE_BG=\033[47m 
RED_BG=\033[41m 
GREEN_BG=\033[42m
YELLOW_BG=\033[43m  
BLUE_BG=\033[44m 
PURPLE_BG=\033[45m
CYAN_BG=\033[46m 
BLACK_BG=\033[40m 
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ## 
##                   VARIABLES                     ## 
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ## 
CXX = g++
CXXFLAGS = -Wall -Werror -Wextra -std=c++17 -O3

COVERAGE = 
LIBS = -lstdc++ -lgtest -lm

## SOURCES = $(wildcard model/*.cc controller/*.cc)
SOURCES = $(wildcard model/*.cc)
OBJECTS = $(patsubst %.cc,%.o,$(notdir $(SOURCES)))

TESTS_FILENAME = unit_tests
TESTS_FOLDER = tests
TESTS_SOURCES = $(wildcard $(TESTS_FOLDER)/*.cc)
TESTS_OBJECTS = $(patsubst %.cc,%.o,$(notdir $(TESTS_SOURCES)))

QT_PROJECT = cpp7_mlp
QT_PROJECT_FOLDER = view

EMU_ROOT = root

## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ## 
##                  APPLICATION                    ##
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ## 
.PHONY: all install uninstall clean dvi dist tests gcov_report debug add_debug add_coverage 

all: clean gcov_report

clean: 
	@find . -type f \( -name "*.o" -o -name "*.out" \) -delete
	@rm -rf report *.gcda *.gcno 
	@rm -rf valgrind.log $(TESTS_FOLDER)/$(TESTS_FILENAME)
	@rm -rfv $(QT_PROJECT_FOLDER)/CMakeLists.txt.user build-* makefile_generated.mk moc_* ui_* $(QT_PROJECT) qrc_resources.cpp
	@rm -rfv $(QT_PROJECT).tar.gz
	@rm -rfv latex html doxygen

install:  
	@echo "\n"
	@echo "$(BLACK_FG)$(GREEN_BG)                                                $(DEFAULT)"
	@echo "$(BLACK_FG)$(GREEN_BG)                    INSTALL                     $(DEFAULT)"
	@echo "$(BLACK_FG)$(GREEN_BG)                                                $(DEFAULT)\n"
	rm -rf build/
	cmake view/ -B build/
	make -C build

uninstall: 
	@echo "\n"
	@echo "$(BLACK_FG)$(YELLOW_BG)                                                $(DEFAULT)"
	@echo "$(BLACK_FG)$(YELLOW_BG)                   UNINSTALL                    $(DEFAULT)"
	@echo "$(BLACK_FG)$(YELLOW_BG)                                                $(DEFAULT)\n"
	@rm -rf build 

dvi:
	@rm -rfv latex html doxygen
	@doxygen Doxyfile
	@-open html/index.html

dist: clean
	tar -cvf $(QT_PROJECT).tar *
	gzip $(QT_PROJECT).tar

tests: 
	@echo "\n"
	@echo "$(BLACK_FG)$(RED_BG)                                                $(DEFAULT)"
	@echo "$(BLACK_FG)$(RED_BG)                  BUILD TESTS                   $(DEFAULT)"
	@echo "$(BLACK_FG)$(RED_BG)                                                $(DEFAULT)\n"
	$(CXX) $(CXXFLAGS) $(COVERAGE) -DTESTS -c $(SOURCES) 
	$(CXX) $(CXXFLAGS) $(COVERAGE) -DTESTS -c $(TESTS_SOURCES) 
	$(CXX) $(CXXFLAGS) $(COVERAGE) -DTESTS $(TESTS_OBJECTS) $(OBJECTS) -o $(TESTS_FOLDER)/$(TESTS_FILENAME) $(LIBS)
	@echo "\n"
	@echo "$(BLACK_FG)$(RED_BG)                                                $(DEFAULT)"
	@echo "$(BLACK_FG)$(RED_BG)                  RUN TESTS                     $(DEFAULT)"
	@echo "$(BLACK_FG)$(RED_BG)                                                $(DEFAULT)\n"

	$(TESTS_FOLDER)/$(TESTS_FILENAME)

gcov_report: add_coverage tests
	@echo "\n"
	@echo "$(BLACK_FG)$(GREEN_BG)                                                $(DEFAULT)"
	@echo "$(BLACK_FG)$(GREEN_BG)                  GCOV REPORT                   $(DEFAULT)"
	@echo "$(BLACK_FG)$(GREEN_BG)                                                $(DEFAULT)\n"
	mkdir -p report
	lcov -no-external -o report/unit_tests.info -c -d .
	genhtml -o report report/unit_tests.info

add_coverage:
	$(eval COVERAGE += --coverage)

## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ##
##                     CHECK                       ##
## ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ##
.PHONY: check clang_format cppcheck cpplint valgrind 
check: clang_format cppcheck valgrind 

clang_format:
	@echo "\n"
	@echo "$(BLACK_FG)$(PURPLE_BG)                                                $(DEFAULT)"
	@echo "$(BLACK_FG)$(PURPLE_BG)                 CLANG-FORMAT                   $(DEFAULT)"
	@echo "$(BLACK_FG)$(PURPLE_BG)                                                $(DEFAULT)\n"
	@echo "Clang-format. Dry-run. Do not actually make the formatting changes."
	clang-format --dry-run --verbose -style=Google model/* controller/*
	@echo "Clang-format. Correction."
	clang-format -i --verbose -style=Google model/* controller/*

cppcheck:
	@echo "\n"
	@echo "$(BLACK_FG)$(CYAN_BG)                                                $(DEFAULT)"
	@echo "$(BLACK_FG)$(CYAN_BG)                   CPPCHECK                     $(DEFAULT)"
	@echo "$(BLACK_FG)$(CYAN_BG)                                                $(DEFAULT)\n"
	cppcheck --std=c++17 --language=c++ --enable=all --suppress=missingIncludeSystem \
	model/*.h model/*.cc controller/*.h controller/*.cc tests/*.h tests/*.cc ##view/*.h view/*.cc 

valgrind:
	@echo "\n"
	@echo "$(BLACK_FG)$(BLUE_BG)                                                $(DEFAULT)"
	@echo "$(BLACK_FG)$(BLUE_BG)                    VALGRIND                    $(DEFAULT)"
	@echo "$(BLACK_FG)$(BLUE_BG)                                                $(DEFAULT)\n"
ifeq ($(UNAME_S),Darwin)
	@echo "\n$(RED_FG)Valgrind does not natively support Mac OS X due to the nature of its architecture.\nPlease run the target on Linux.$(DEFAULT)\n" 
else
ifeq ("$(wildcard $(TESTS_FOLDER)/$(TESTS_FILENAME))","")
	@echo "\n$(RED_FG)Run 'make test' before check.$(DEFAULT)\n" 
else 
	valgrind --leak-check=full --log-file=valgrind.log $(TESTS_FOLDER)/$(TESTS_FILENAME) 
	@echo "\n"
	@echo "$(BLACK_FG)$(BLUE_BG)                                                $(DEFAULT)"
	@echo "$(BLACK_FG)$(BLUE_BG)             VALGRIND RESULT FILTER             $(DEFAULT)"
	@echo "$(BLACK_FG)$(BLUE_BG)                                                $(DEFAULT)\n"
	grep "ERROR SUMMARY:" valgrind.log
endif 
endif 