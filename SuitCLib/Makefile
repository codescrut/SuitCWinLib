# Compiler and Linker
CC = gcc
CXX = g++

# User-definable SUITCAPP, defaults to 0 if not set on command line or in environment
SUITCAPP ?= 1

# Compile flags
# -fPIC: Position Independent Code for shared libraries
# -Wall -Wextra: Enable all warnings
# -I.: Add current directory to include path
# -g: Add debug symbols (optional, but good for development)
# -DSUITCAPP=0: Example of defining SUITCAPP, change value as needed or remove if set elsewhere
#               You can override this on the command line: make SUITCAPP=1
CFLAGS = -fPIC -Wall -Wextra -I. -g -DSUITCAPP=$(SUITCAPP)
CXXFLAGS = -fPIC -Wall -Wextra -I. -g -DSUITCAPP=$(SUITCAPP)

# Linker flags
# -shared: Create a shared library
# -fvisibility=hidden: Default symbol visibility to hidden
LDFLAGS = -shared -fvisibility=hidden -g

# Name of the output shared library
TARGET_LIB = libsuitc.so

# List of C source files
# Exclude suitc_globals.c as it's handled separately for clarity, though it could be in this list.
# Exclude dllmain.cpp and pch.cpp as they are likely Windows-specific or handled by includes.
C_SOURCES = \
    add_node.c \
    add_quirk.c \
    apply_played_tricks.c \
    build_selected_tree.c \
    check_ew_plays.c \
    check_ns_plays.c \
    check_quirkd.c \
    check_zermelo.c \
    clean_quirk.c \
    convert_tree_to_quirk.c \
    copy_node.c \
    copy_tree.c \
    count_quirk.c \
    create_all_boards.c \
    create_board_header.c \
    create_boards.c \
    create_game_tree_json_value.c \
    create_ply1.c \
    create_ply2.c \
    create_ply3.c \
    create_ply4.c \
    create_tricks.c \
    delete_board.c \
    delete_quirk.c \
    delete_tree.c \
    determine_trick_count.c \
    evaluate_best_results.c \
    evaluate_mp_results.c \
    evaluate_mp_scores.c \
    fold_ew_tree.c \
    fold_ns_tree.c \
    fold_quirk.c \
    fold_tree.c \
    limit_matchpoint_nodes.c \
    limit_quirk.c \
    merge_nodes.c \
    merge_quirk.c \
    minimax.c \
    print_all_results.c \
    print_boards.c \
    print_node.c \
    print_quirk.c \
    print_results.c \
    print_stat.c \
    print_suitc.c \
    print_summary_results.c \
    print_tree.c \
    process_args.c \
    select_ew_plays.c \
    select_ns_plays.c \
    suitc.c \
    use_suit_entries.c

# C++ source files
CPP_SOURCES = \
    lib_main.cpp

# Global variables definition file
GLOBALS_C_SOURCE = suitc_globals.c

# Object files
C_OBJECTS = $(C_SOURCES:.c=.o)
CPP_OBJECTS = $(CPP_SOURCES:.cpp=.o)
GLOBALS_OBJECT = $(GLOBALS_C_SOURCE:.c=.o)

ALL_OBJECTS = $(GLOBALS_OBJECT) $(C_OBJECTS) $(CPP_OBJECTS)

# Header files (for dependency tracking)
# Add all relevant .h files. `header.h` is the main one.
HEADERS = header.h bump_codes.h # Add pch.h, framework.h if they are actual dependencies

# Default target
all: $(TARGET_LIB)

# Rule to link the shared library
$(TARGET_LIB): $(ALL_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(ALL_OBJECTS)

# Rule to compile C source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile C++ source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile the global definitions C file
$(GLOBALS_OBJECT): $(GLOBALS_C_SOURCE) $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


# Clean rule
clean:
	rm -f $(ALL_OBJECTS) $(TARGET_LIB)

# Phony targets
.PHONY: all clean
