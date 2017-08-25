/*
  This program helps you to remember shortcuts for a specific program. 
  These shortcuts can grouped into a specific category.
  Thus, you can display all shortcuts or a specific category.
  Author: Boissier Florian
  Compilation:
  g++ -o shrm shrm.cpp -Wall -Wextra -std=c++17
  sudo ln -s shrm /usr/bin
*/

/*
  Creates a .txt in /usr/share/shrm
  For instance /usr/share/shrm/emacs.txt
  Example:
  
  (C-x s) (Saves all files)
  
  [category]
  (C-x C-s) (Save the current buffer)
  (C-x C-f) (Open a new file in the current buffer)
  
  [category2]
  ...
  
*/

#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <vector>

struct Shortcut
{
  Shortcut() :
    mCategory(""),
    mBinding(""),
    mDescription("")
  {
  }

  Shortcut(const std::string& category, const std::string& binding):
    mCategory(category),
    mBinding(binding),
    mDescription("")
  {

  }

  
  Shortcut(const std::string& category, const std::string& binding, const std::string& description) :
    mCategory(category),
    mBinding(binding),
    mDescription(description)
  {

  }
  
  Shortcut(const std::string& binding) :
    mCategory(""),
    mBinding(binding),
    mDescription("")
  {
    
  }

  void print() const
  {
    std::cout << "[" << mCategory << "] " << mBinding << " (" << mDescription << ")" << std::endl;
  }
  
  std::string mCategory;
  std::string mBinding;
  std::string mDescription;
};

std::vector<Shortcut> shortcuts;

void load_file(const std::string& program)
{
  // Check input
  std::ifstream ifstream("/usr/share/shrm/" + program + ".txt");
  if (!ifstream)
  {
    std::cerr << "Programme inexistant: " << program  << std::endl;
    return;
  }
  
  std::string string;
  std::regex binding_regex("\\((.+)\\)\\s*\\((.+)\\)");
  std::regex category_regex("\\[(.+)\\]");
  std::smatch base_match;
  std::string category("");
  int line(1);
  bool category_flag(false);
  
  for (std::string string; std::getline(ifstream, string);)
  {
   
    if (std::regex_match(string, base_match, category_regex))
    {
      if (base_match.size() == 2)
      {
	std::ssub_match sub_match = base_match[1];
	category = sub_match.str();
	category_flag = true;
      }
      else
      {
	std::cerr << "Can't parse category at line " << line << std::endl; 
      }
    }


    // Looking for a binding
    if (std::regex_match(string, base_match, binding_regex))
    {
      if (base_match.size() == 3)
      {
	std::ssub_match sub_match = base_match[1];
	std::string binding = sub_match.str();
	sub_match = base_match[2];
	std::string description = sub_match.str();

	Shortcut s("", binding, description);

	if (category_flag)
	  s.mCategory = category;
	  
	shortcuts.push_back(s);
      }
      else
      {
	std::cerr << "Can't parse binding at line " << line << std::endl; 
      }
    }
    
    line++;
  }
}

void match_smth(const std::string& reg)
{
  std::regex base_regex(reg);
  std::smatch smatch;
  bool matched_smth(false);
  
  for (const Shortcut& s : shortcuts)
  {
    if (std::regex_search(s.mCategory, smatch, base_regex))
    {
      s.print();
      matched_smth = true;
    }
    
  }

  base_regex.assign(reg, std::regex_constants::icase);

  if (!matched_smth)
  {
    for (const Shortcut& s : shortcuts)
      if (std::regex_search(s.mDescription, smatch, base_regex))
      {
	s.print();
	matched_smth = true;
      }
  }

  if (!matched_smth)
  {
    for (const Shortcut& s : shortcuts)
      if (std::regex_search(s.mBinding, smatch, base_regex))
	s.print();
  }
}

void print_all()
{
  for (const Shortcut& s : shortcuts)
    s.print();
}


int main(int argc, char** argv)
{
  // Read arguments
  if (argc == 1 or argc > 3)
  {
    std::cout << "Usage: shrm <program> [category|regex]" << std::endl;
    return 0;
  }

  load_file(argv[1]);

  // Global mode
  if (argc == 2)
  {
    print_all();
  }
  // Category/Regex mode
  else if (argc == 3)
  {
    match_smth(argv[2]); 
  }

  return 0;
}
