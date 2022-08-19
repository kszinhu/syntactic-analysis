/**
 * @Nome: Cassiano Henrique Aparecido Rodrigues
 * @Matricula: 201020891
 * @Sistema Operacional: Manjaro Linux (5.13.19-2-MANJARO-x86_64)
 * @Compilador: gcc (GCC) 12.1.0
 *
 * Using ISO C++17 (C++17) -- apply with -std=c++17 flag
 */
#include <map>
#include <iostream>
#include <string>
#include <fstream>

std::ifstream input_file;
std::ofstream output_file;

int line_number = 1, column_number = 1;

std::map<std::string, std::string> tokens = {
    {"PROGRAM", "RW_PROGRAM"},
    {"VAR", "RW_VAR"},
    {"CONST", "RW_CONST"},
    {"PROCEDURE", "RW_PROCEDURE"},
    {"FUNCTION", "RW_FUNCTION"},
    {"LABEL", "RW_LABEL"},
    {"BEGIN", "RW_BEGIN"},
    {"DOWNTO", "RW_DOWNTO"},
    {"END", "RW_END"},
    {"IF", "RW_IF"},
    {"THEN", "RW_THEN"},
    {"ELSE", "RW_ELSE"},
    {"RAISE", "RW_RAISE"},
    {"CATCH", "RW_CATCH"},
    {"TRY", "RW_TRY"},
    {"FINALLY", "RW_FINALLY"},
    {"RECORD", "RW_RECORD"},
    {"REPEAT", "RW_REPEAT"},
    {"TYPE", "RW_TYPE"},
    {"UNTIL", "RW_UNTIL"},
    {"USES", "RW_USES"},
    {"WHILE", "RW_WHILE"},
    {"FOR", "RW_FOR"},
    {"DO", "RW_DO"},
    {"OR", "RW_OR"},
    {"IN", "RW_IN"},
    {"AND", "RW_AND"},
    {"NOT", "RW_NOT"},
    {"DIV", "RW_DIV"},
    {"INTEGER", "RW_INTEGER"},
    {"ARRAY", "RW_ARRAY"},
    {"STRING", "RW_STRING"},
    {":=", "SS_ASSIGN"},
    {"+", "SS_PLUS"},
    {"-", "SS_MINUS"},
    {"*", "SS_MULTIPLY"},
    {"/", "SS_DIVIDE"},
    {"<", "SS_LESS"},
    {">", "SS_GREATER"},
    {"<=", "SS_LESS_EQUAL"},
    {">=", "SS_GREATER_EQUAL"},
    {"=", "SS_EQUAL"},
    {"(", "SS_OPEN_PARENTHESIS"},
    {")", "SS_CLOSE_PARENTHESIS"},
    {".", "SS_DOT"},
    {",", "SS_COMMA"},
    {";", "SS_SEMICOLON"},
    {":", "SS_COLON"},
};

void write_output(std::string token)
{
  output_file << std::noskipws << token << ' ';
}
void write_output(char capsuled_char)
{
  output_file << std::noskipws << capsuled_char;
}

// FUNÇÃO PRÓXIMO
bool proximo(char &next_char)
{
  if (input_file.good() && input_file.peek() != EOF)
  {
    input_file >> std::noskipws >> next_char;

    if (next_char == '\n')
    {
      line_number++;
      column_number = 1;
    }
    else if (next_char == '\t')
    {
      column_number += 4;
    }
    else
    {
      column_number++;
      if (next_char >= 'a' && next_char <= 'z')
        next_char = toupper(next_char);
    }

    return true;
  }

  return false;
}

// CODIGO (TOKENIZADOR)
std::string codigo(std::string atom, int type)
{
  switch (type)
  {
  case 1:
    return "NUMBER_" + atom;

  case 2:
    return "IDENTIFIER_" + atom;

  default:
    return tokens[atom];
  }
}

// FUNÇÃO ERRO
void erro()
{
  throw std::runtime_error("Token inválido na linha " + std::to_string(line_number) + " na posição " + std::to_string(column_number));
}

std::string lexical_analysis(char &next_char)
{
  std::string atom = "";

  while (next_char == '\n' || next_char == '\t' || next_char == ' ')
  {
    if (next_char != ' ')
      write_output(next_char);

    proximo(next_char);
  }

  if (tokens[std::string(1, next_char)] != "")
  {
    atom = next_char;
    proximo(next_char);

    if (atom == ":" && next_char == '=')
    {
      atom += next_char;
      proximo(next_char);
    }

    return codigo(atom, 0);
  }
  else
  {
    if (next_char <= 'Z' && next_char >= 'A')
    {
      do
      {
        atom += next_char;
        proximo(next_char);
      } while (next_char <= 'Z' && next_char >= 'A' || next_char <= '9' && next_char >= '0');
      if (tokens[atom] != "")
        return codigo(atom, 0); // reserved word
      else
        return codigo(atom, 2); // identifier
    }
    else
    {
      if (next_char <= '9' && next_char >= '0')
      {
        do
        {
          atom += next_char;
          proximo(next_char);
        } while (next_char <= '9' && next_char >= '0');

        if (next_char <= 'Z' && next_char >= 'A')
        {
          erro();
        }

        return codigo(atom, 1); // number
      }
      else
      {
        erro();
      }
    }
  }

  return ""; // unreachable
}

int main()
{
  input_file.open("programa.txt");
  output_file.open("output.out");

  char next_char;
  proximo(next_char);

  try
  {
    do
    {
      write_output(lexical_analysis(next_char));
    } while (input_file.good());
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  input_file.close();
  output_file.close();

  return 0;
}
