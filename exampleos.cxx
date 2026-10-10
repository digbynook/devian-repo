
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define PURPLE  "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define RESET   "\033[0m"

// Devian v0.5.1
// ebuild edits and saves recipes.
// build creates a package.
// emerge installs an already-built package.

void editSection(const std::string& title, std::string& content) {
    std::cout << CYAN << "\n--- " << title << " ---\n"
              << RESET;
    std::cout << "Enter text one line at a time.\n";
    std::cout << "Enter a single . on its own line to finish.\n";

    std::string line;
    content.clear();

    while (true) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) {
            return;
        }

        if (line == ".") {
            break;
        }

        content += line + '\n';
    }
}

void ebuild(const std::string& name) {
    if (name.empty()) {
        std::cout << RED
                  << "Usage: ebuild <package>\n"
                  << RESET;
        return;
    }

    if (name.find('/') != std::string::npos ||
        name.find('\\') != std::string::npos ||
        name == "." || name == "..") {
        std::cout << RED << "Invalid package name.\n" << RESET;
        return;
    }

    std::string coding;
    std::string instructions;

    // Load an existing recipe so it can be edited.
    std::ifstream existing(name + ".erecipe");

    if (existing.is_open()) {
        std::string line;
        std::string* section = nullptr;

        while (std::getline(existing, line)) {
            if (line == "[Coding]") {
                section = &coding;
            } else if (line == "[Recipe instruction]") {
                section = &instructions;
            } else if (section != nullptr) {
                *section += line + '\n';
            }
        }
    }

    while (true) {
        std::cout << CYAN
                  << "\nDevian ebuild editor: " << name << "\n"
                  << RESET;

        std::cout << "1. Coding\n";
        std::cout << "2. Recipe instruction\n";
        std::cout << "3. Save recipe\n";
        std::cout << "4. Exit without saving\n";
        std::cout << "Select: ";

        std::string choice;

        if (!std::getline(std::cin, choice)) {
            return;
        }

        if (choice == "1") {
            editSection("Coding", coding);
        } else if (choice == "2") {
            editSection("Recipe instruction", instructions);
        } else if (choice == "3") {
            std::ofstream recipe(name + ".erecipe");

            if (!recipe.is_open()) {
                std::cout << RED
                          << "Could not save recipe: "
                          << name << ".erecipe\n"
                          << RESET;
                continue;
            }

            recipe << "[Coding]\n";
            recipe << coding;

            if (!coding.empty() && coding.back() != '\n') {
                recipe << '\n';
            }

            recipe << "[Recipe instruction]\n";
            recipe << instructions;

            recipe.close();

            if (!recipe) {
                std::cout << RED
                          << "Error saving recipe.\n"
                          << RESET;
            } else {
                std::cout << GREEN
                          << "Recipe saved: " << name << ".erecipe\n"
                          << RESET;
                return;
            }
        } else if (choice == "4") {
            std::cout << YELLOW
                      << "Exited without saving.\n"
                      << RESET;
            return;
        } else {
            std::cout << RED << "Invalid selection.\n" << RESET;
        }
    }
}

void buildPackage(const std::string& name) {
    if (name.empty()) {
        std::cout << RED
                  << "Usage: build <package>\n"
                  << RESET;
        return;
    }

    std::string recipeFile = name + ".erecipe";
    std::string packageFile = name + ".pkg";

    std::ifstream recipe(recipeFile);

    if (!recipe.is_open()) {
        std::cout << RED
                  << "Recipe not found: " << recipeFile << '\n'
                  << "Create it with ebuild " << name << '\n'
                  << RESET;
        return;
    }

    std::ofstream package(packageFile);

    if (!package.is_open()) {
        std::cout << RED
                  << "Could not create package: " << packageFile << '\n'
                  << RESET;
        return;
    }

    std::cout << CYAN
              << "Building package: " << name << '\n'
              << RESET;

    package << "# Devian simulated package\n";
    package << "# Package: " << name << '\n';
    package << "# Source recipe: " << recipeFile << "\n\n";

    package << recipe.rdbuf();

    recipe.close();
    package.close();

    if (!package) {
        std::cout << RED
                  << "Failed to write package.\n"
                  << RESET;
        return;
    }

    std::cout << GREEN
              << "Package built: " << packageFile << '\n'
              << RESET;

    std::cout << YELLOW
              << "Note: building is simulated; source code is not compiled yet.\n"
              << RESET;
}

int main() {
    std::string input;
    std::string password;
    std::string user = "admin";

    bool running = true;
    bool sudoAuthenticated = false;

    auto sudoExpiry = std::chrono::steady_clock::now();

    std::cout << CYAN
              << "Devian Terminal v0.5.1\n"
              << "Debian's baby with a Gentoo package manager.\n"
              << RESET;

    while (running) {
        if (user == "root") {
            std::cout << RED << "Root > " << RESET;
        } else {
            std::cout << GREEN << "Admin > " << RESET;
        }

        if (!std::getline(std::cin, input)) {
            break;
        }

        std::istringstream stream(input);
        std::string command;
        std::string argument;

        stream >> command >> argument;

        if (command.empty()) {
            continue;
        }

        // HELP
        else if (command == "help") {
            std::cout << CYAN
                      << "\nDevian Commands\n"
                      << RESET;

            std::cout << WHITE
                      << "help       - Show commands\n"
                      << "whoami     - Show current user\n"
                      << "neofetch   - Show system information\n"
                      << "su         - Switch to root\n"
                      << "sudo       - Authenticate for sudo\n"
                      << "ebuild     - Edit and save a package recipe\n"
                      << "build      - Build a package from a recipe\n"
                      << "emerge     - Install a built package\n"
                      << "exit       - Return to admin or quit\n"
                      << RESET;
        }

        // WHOAMI
        else if (command == "whoami") {
            std::cout << user << '\n';
        }

        // NEOFETCH -- PRESERVED FROM v0.5
        else if (command == "neofetch") {
            const char* swirl[] = {
                "       _,met$$$$$gg.",
                "    ,g$$$$$$$$$$$$$$$P.",
                "  ,g$$P\"     \"\"\"Y$$.\".",
                " ,$$P'              `$$$.",
                "',$$P       ,ggs.     `$$b:",
                "`d$$'     ,$P\"'   .    $$$",
                " $$P      d$'     ,    $$P",
                " $$:      $$.   -    ,d$$'",
                " $$;      Y$b._   _,d$P'",
                " Y$$.    `.`\"Y$$$$P\"'",
                " `$$b      \"-.__",
                "  `Y$$",
                "   `Y$$.",
                "     `$$b.",
                "       `Y$$b.",
                "          `\"Y$b._"
            };

            const char* info[] = {
                "user@devian",
                "------------",
                "OS: Devian Linux",
                "Kernel: Linux",
                "Shell: Devian Terminal",
                "Release: 0.5",
                "Package Manager: emerge",
                "Build Tool: ebuild",
                "Architecture: 64-bit",
                "Language: C++"
            };

            int swirlLines = sizeof(swirl) / sizeof(swirl[0]);
            int infoLines = sizeof(info) / sizeof(info[0]);
            int totalLines =
                swirlLines > infoLines ? swirlLines : infoLines;

            for (int i = 0; i < totalLines; ++i) {
                if (i < swirlLines) {
                    std::cout << RED << swirl[i] << RESET;
                } else {
                    std::cout << "                     ";
                }

                if (i < infoLines) {
                    std::cout << "   ";

                    if (i < 2) {
                        std::cout << PURPLE << info[i] << RESET;
                    } else {
                        std::cout << CYAN << info[i] << RESET;
                    }
                }

                std::cout << '\n';
            }

            // Equal-width color blocks; no extra trailing block.
            std::cout
                << "\033[41m  "
                << "\033[42m  "
                << "\033[43m  "
                << "\033[44m  "
                << "\033[45m  "
                << "\033[46m  "
                << "\033[47m  "
                << RESET << '\n';
        }

        // SU
        else if (command == "su") {
            if (user == "root") {
                std::cout << YELLOW
                          << "Already root!\n"
                          << RESET;
            } else {
                std::cout << "Enter password: ";

                if (!std::getline(std::cin, password)) {
                    break;
                }

                if (password == "mangos123") {
                    user = "root";

                    std::cout << GREEN
                              << "Access granted.\n"
                              << RESET;
                } else {
                    std::cout << RED
                              << "Access denied.\n"
                              << RESET;
                }
            }
        }

        // SUDO
        else if (command == "sudo") {
            if (user == "root") {
                std::cout << YELLOW
                          << "Already root!\n"
                          << RESET;
            } else {
                auto now = std::chrono::steady_clock::now();

                if (sudoAuthenticated && now < sudoExpiry) {
                    std::cout << GREEN
                              << "Sudo authentication is still valid.\n"
                              << RESET;
                } else {
                    sudoAuthenticated = false;

                    std::cout << "Enter password for sudo: ";

                    if (!std::getline(std::cin, password)) {
                        break;
                    }

                    if (password == "mangos123") {
                        sudoAuthenticated = true;
                        sudoExpiry =
                            std::chrono::steady_clock::now()
                            + std::chrono::minutes(5);

                        std::cout << GREEN
                                  << "Sudo authentication successful.\n"
                                  << RESET;

                        std::cout << YELLOW
                                  << "Authentication cached for 5 minutes.\n"
                                  << "No privileged operations configured yet.\n"
                                  << RESET;
                    } else {
                        std::cout << RED
                                  << "Sudo authentication failed.\n"
                                  << RESET;
                    }
                }
            }
        }

        // EBUILD -- OPENS THE TWO-SECTION EDITOR
        else if (command == "ebuild") {
            ebuild(argument);
        }

        // BUILD -- CREATES A SIMULATED PACKAGE
        else if (command == "build") {
            buildPackage(argument);
        }

        // EMERGE -- ONLY INSTALLS AN EXISTING PACKAGE
        else if (command == "emerge") {
            if (argument.empty()) {
                std::cout << RED
                          << "Usage: emerge <package>\n"
                          << RESET;
            } else {
                std::string packageFile = argument + ".pkg";
                std::ifstream package(packageFile);

                if (!package.is_open()) {
                    std::cout << RED
                              << "Package not built: "
                              << argument
                              << "\nRun build "
                              << argument
                              << " first.\n"
                              << RESET;
                } else {
                    package.close();

                    std::cout << GREEN
                              << "Installing package: "
                              << argument
                              << RESET << '\n';

                    std::cout << YELLOW
                              << "Installation simulated; "
                              << "system files were not changed.\n"
                              << RESET;
                }
            }
        }

        // EXIT
        else if (command == "exit") {
            if (user == "root") {
                user = "admin";

                std::cout << YELLOW
                          << "Returned to admin.\n"
                          << RESET;
            } else {
                std::cout << GREEN
                          << "Goodbye!\n"
                          << RESET;

                running = false;
            }
        }

        // UNKNOWN COMMAND
        else {
            std::cout << RED
                      << "Unknown command: "
                      << command
                      << RESET << '\n';

            std::cout << "Type 'help' for commands.\n";
        }
    }

    return 0;
}
	