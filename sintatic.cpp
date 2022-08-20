/**
 * @Nome: Cassiano Henrique Aparecido Rodrigues
 * @Matricula: 201020891
 * @Sistema Operacional: Manjaro Linux (5.13.19-2-MANJARO-x86_64)
 * @Compilador: gcc (GCC) 12.1.0
 *
 * Using ISO C++17 (C++17) -- apply with -std=c++17 flag
 */
#include "lexical.h"
#include <queue>
#include <regex>
#include <locale>

#define CURRENT_TOKEN std::get<2>(queueTokens.front())
#define IS_IDENTIFIER std::regex_match(CURRENT_TOKEN, std::regex("IDENTIFIER_.*"))
#define IS_NUMBER std::regex_match(CURRENT_TOKEN, std::regex("NUMBER_.*"))

std::queue<std::tuple<int, int, std::string>> queueTokens;

// Headers das funções
// -------------------

// Análise Sintática
void sintaticAnalysis();

// Funções auxiliares:
void identifyToken(char &next_char);
void fillQueueWithTokens();
void next(); // Pula para o próximo elemento da fila
void throwError(std::string message);

// Funções de análise sintática
void program();
void block();
void partDeclaration();
void declaration();
void type();
void listIdentifier();
void auxDeclaration();
void declarationProcedure();
void declarationFunction();
void partDeclarationSubroutine();
void formalParameters();
void sectionFormalParameters();
void partSectionFormalParameters();
void listIdentifierFormalParameters();
void endSectionFormalParameters();
void compoundStatement();
void statement();
void endStatement();
void statementWithoutLabel();
void assignmentOrProcedure();
void assignment();
void callProcedure();
void ifStatement();
void endConditionalStatement();
void whileStatement();
void forStatement();
void expression();
void simpleExpression();
void optionalExpression();
void relationalExpression();
void listOfExpressions();
void term();
void factor();
void factorWithExpression();

// -------------------
// Funções auxiliares
// -------------------
void identifyToken(char &next_char)
{

  std::string token = "";
  while (next_char != ' ')
  {
    // Enquanto o carácter não for um espaço, vai capturar a palavra
    if (next_char != '\t' && next_char != '\n')
      token += next_char;
    proximo(next_char);
  }

  // A cada palavra capturada, vai ser inserido no topo da pilha com seu número de linha e coluna
  // column_number é a coluna do carácter atual, e é decrementado o número de colunas usadas para o tamanho do token (fica mais próximo do real)
  queueTokens.push(std::make_tuple(line_number, column_number - token.size(), token));
}

void fillQueueWithTokens()
{
  char next_char;
  // Preencher pilha de tokens com os tokens do arquivo
  try
  {
    int line_number = 1;
    do
    {
      proximo(next_char);
      identifyToken(next_char);
    } while (read_file.good());
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }
}

void throwError(std::string message, std::tuple<int, int, std::string> token)
{
  // Imprime a mensagem de erro e a linha e coluna do token que gerou o erro, mais a mensagem de erro;
  throw std::runtime_error("Erro na linha " + std::to_string(std::get<0>(token)) + " coluna " + std::to_string(std::get<1>(token)) + ": " + message + '\n');
}

void next(std::string token, int type)
{
  if (type == 1) // Nomeados, verifica o tipo
  {
    std::string tokenType = std::regex_replace(token, std::regex("_.*"), "");
    std::string currentTokenType = std::regex_replace(CURRENT_TOKEN, std::regex("_.*"), "");

    if (tokenType == currentTokenType)
    {
      // Avança para o próximo elemento da fila:
      queueTokens.pop();
    }
    else
    {
      // Token não é o esperado, deve-se gerar um erro:
      throwError("(next): Esperado " + token + " encontrado " + CURRENT_TOKEN, queueTokens.front());
    }
  }
  else
  { // Não nomeados verifica o token
    if (std::regex_match(CURRENT_TOKEN, std::regex(token)))
    {
      // Avança para o próximo elemento da fila:
      queueTokens.pop();
    }
    else
    {
      // Token não é o esperado, deve-se gerar um erro:
      throwError("(next): Esperado " + token + " encontrado " + CURRENT_TOKEN, queueTokens.front());
    }
  }
}
// -------------------

void statement()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("NUMBER_.*")))
  {
    next("NUMBER", 1);
    next("SS_COLON", 2); // dois pontos
  }

  statementWithoutLabel();
}

void program()
{
  std::tuple<int, int, std::string> currentToken = queueTokens.front();

  if (std::regex_match(std::get<2>(currentToken), std::regex("RW_PROGRAM")))
  {
    next("RW_PROGRAM", 2);
    next("IDENTIFIER", 1);
    next("SS_SEMICOLON", 2);
    block();
    next("SS_DOT", 2);
  }
  else
  {
    throwError("Erro sintático - Esperado 'PROGRAM' encontrado " + std::get<2>(currentToken), queueTokens.front());
  }
}

void block()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_VAR")))
  {
    partDeclaration();
  }
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_PROCEDURE|RW_FUNCTION")))
  {
    partDeclarationSubroutine();
  }
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_BEGIN")))
  {
    compoundStatement();
  }
}

void partDeclaration()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_VAR")))
  {
    next("RW_VAR", 2);
    declaration();
  }
  else
  {
    throwError("Erro sintático - Esperado 'VAR' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void declaration()
{

  if (IS_IDENTIFIER)
  {
    next("IDENTIFIER", 1);
    listIdentifier();
  }
  else
  {
    throwError("Erro sintático - Esperado um identificador encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void listIdentifier()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("SS_COMMA")))
  {
    next("SS_COMMA", 2);
    declaration();
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("SS_COLON")))
  {
    next("SS_COLON", 2);
    type();
    next("SS_SEMICOLON", 2);
    auxDeclaration();
  }
  else
  {
    throwError("Erro sintático - Esperado ',' ou ';' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void type()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_INTEGER|RW_BOOLEAN")))
  {
    next("RW_INTEGER|RW_BOOLEAN", 2);
  }
  else
  {
    throwError("Erro sintático - Esperado 'INTEGER' ou 'BOOLEAN' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void auxDeclaration()
{
  if (IS_IDENTIFIER)
  {
    next("IDENTIFIER", 1);
    declaration();
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("RW_PROCEDURE|RW_FUNCTION|RW_BEGIN")))
  {
    block();
  }
  else
  {
    throwError("Erro sintático - Esperado 'IDENTIFIER' ou 'PROCEDURE' ou 'FUNCTION' ou 'BEGIN' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void partDeclarationSubroutine()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_PROCEDURE")))
  {
    next("RW_PROCEDURE", 2);
    declarationProcedure();
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("RW_FUNCTION")))
  {
    next("RW_FUNCTION", 2);
    declarationFunction();
  }
  else
  {
    throwError("Erro sintático - Esperado 'PROCEDURE' ou 'FUNCTION' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void declarationProcedure()
{
  if (IS_IDENTIFIER)
  {
    next("IDENTIFIER", 1);
    formalParameters();
    next("SS_SEMICOLON", 2);
    block();
  }
  else
  {
    throwError("Erro sintático - Esperado 'IDENTIFIER' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void declarationFunction()
{

  if (IS_IDENTIFIER)
  {
    next("IDENTIFIER", 1);
    formalParameters();
    next("SS_COLON", 2);
    next("IDENTIFIER", 1);
    next("SS_SEMICOLON", 2);
    block();
  }
  else
  {
    throwError("Erro sintático - Esperado 'IDENTIFIER' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void formalParameters()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("SS_OPEN_PARENTHESIS")))
  {
    next("SS_OPEN_PARENTHESIS", 2);
    sectionFormalParameters();
    next("SS_CLOSE_PARENTHESIS", 2);
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("SS_COLON")))
  {
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("SS_SEMICOLON")))
  {
  }
  else
  {
    throwError("Erro sintático - Esperado '(' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void sectionFormalParameters()
{

  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_VAR")))
  {
    next("RW_VAR", 2);
    partSectionFormalParameters();
  }
  else if (IS_IDENTIFIER)
  {
    partSectionFormalParameters();
  }
  else
  {
    throwError("Erro sintático - Esperado 'IDENTIFIER' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void partSectionFormalParameters()
{
  if (IS_IDENTIFIER)
  {
    next("IDENTIFIER", 1);
    listIdentifierFormalParameters();
  }
  else
  {
    throwError("Erro sintático - Esperado 'IDENTIFIER' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void listIdentifierFormalParameters()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("SS_COMMA")))
  {
    next("SS_COMMA", 2);
    partSectionFormalParameters(); // + Mais um identificador
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("SS_COLON")))
  {
    next("SS_COLON", 2);
    next("IDENTIFIER", 1);
    endSectionFormalParameters();
  }
  else
  {
    throwError("Erro sintático - Esperado ',' ou ':' ou ';' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void endSectionFormalParameters()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("SS_SEMICOLON")))
  {
    next("SS_SEMICOLON", 2);
    partSectionFormalParameters(); // + Mais um identificador
  }
}

void compoundStatement()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_BEGIN")))
  {
    next("RW_BEGIN", 2);
    statement();
    while (std::regex_match(CURRENT_TOKEN, std::regex("SS_SEMICOLON")))
    {
      next("SS_SEMICOLON", 2);
      statement();
    }
    next("RW_END", 2);
  }
  else
  {
    throwError("Erro sintático - Esperado BEGIN encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void statementWithoutLabel()
{
  if (IS_IDENTIFIER)
  {
    next("IDENTIFIER", 1);
    if (std::regex_match(CURRENT_TOKEN, std::regex("SS_ASSIGN")))
    {
      assignment();
    }
    else
    {
      callProcedure();
    }
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("RW_END")))
  {
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("RW_BEGIN")))
  {
    compoundStatement();
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("RW_IF")))
  {
    ifStatement();
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("RW_WHILE")))
  {
    whileStatement();
  }
  else
  {
    throwError("Erro sintático - Esperado um comando, encontrado: " + CURRENT_TOKEN, queueTokens.front());
  }
}

void assignment()
{

  if (!std::regex_match(CURRENT_TOKEN, std::regex("SS_ASSIGN")))
  {
    throwError("Erro sintático - Esperado ':=' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }

  next("SS_ASSIGN", 2);
  expression();
}

void callProcedure()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("SS_OPEN_PARENTHESIS")))
  {
    next("SS_OPEN_PARENTHESIS", 2);
    expression();
    next("SS_CLOSE_PARENTHESIS", 2);
  }
}

void ifStatement()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_IF")))
  {
    next("RW_IF", 2);
    expression();
    next("RW_THEN", 2);
    statementWithoutLabel();
    if (std::regex_match(CURRENT_TOKEN, std::regex("RW_ELSE")))
    {
      next("RW_ELSE", 2);
      statementWithoutLabel();
    }
  }
  else
  {
    throwError("Erro sintático - Esperado 'if' encontrado " + CURRENT_TOKEN, queueTokens.front());
  }
}

void factor()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_NOT")))
  {
    // *: Verificar se é um factor "not <factor>"
    next("RW_NOT", 2);
  }

  if (IS_IDENTIFIER)
  { // *: <identifier>
    next("IDENTIFIER", 1);
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("NUMBER_.*")))
  { // *: <number>
    next("NUMBER", 1);
  }
  else if (std::regex_match(CURRENT_TOKEN, std::regex("SS_OPEN_PARENTHESIS")))
  { // *: ( <expression> )
    next("SS_OPEN_PARENTHESIS", 2);
    expression();
    next("SS_CLOSE_PARENTHESIS", 2);
  }
  else
  {
    throwError("Fator inválido", queueTokens.front());
  }
}

void factorWithExpression()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("SS_MULTIPLY|SS_DIVIDE|RW_DIV|RW_AND")))
  {
    next("SS_MULTIPLY|SS_DIVIDE|RW_DIV|RW_AND", 2);
    factor();
  }
}

void term()
{
  factor();
  factorWithExpression();
}

void simpleExpression()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("SS_PLUS|SS_MINUS")))
  {
    next("SS_PLUS|SS_MINUS", 2);
  }

  term();
  optionalExpression();
}

void optionalExpression()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("SS_PLUS|SS_MINUS|RW_OR")))
  {
    next("SS_PLUS|SS_MINUS|RW_OR", 2);
    term();
  }
}

void relationalExpression()
{
  std::string isRelation = "SS_LESS|SS_GREATER|SS_EQUAL|SS_NOT_EQUAL";

  if (std::regex_match(CURRENT_TOKEN, std::regex(isRelation)))
  {
    if (std::regex_match(CURRENT_TOKEN, std::regex("SS_GREATER|SS_LESS")))
    {
      next("SS_LESS|SS_GREATER", 2);
      if (std::regex_match(CURRENT_TOKEN, std::regex("SS_EQUAL")))
      {
        next("SS_EQUAL", 2);
      }
    }
    else
    {
      next(isRelation, 2);
    }

    simpleExpression();
  }
}

void listOfExpressions()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("SS_COMMA")))
  {
    next("SS_COMMA", 2);
    expression();
  }
}

void expression()
{
  simpleExpression();
  relationalExpression();
}

void whileStatement()
{
  if (std::regex_match(CURRENT_TOKEN, std::regex("RW_WHILE")))
  {
    next("RW_WHILE", 2);
    expression();
    next("RW_DO", 2);
    statementWithoutLabel();
  }
  else
  {
    throwError("While inválido", queueTokens.front());
  }
}

// -------------------
// Análise Sintática
// -------------------
void sintaticAnalysis()
{
  program();
}

// -------------------

main()
{
  setlocale(LC_ALL, ".OCP");
  std::string input_file_name;

  std::cout << "O arquivo de entrada está na mesma pasta do executável? (Y/n)" << '\n';
  if (std::toupper(std::cin.get()) == 'N')
  {
    std::cout << "Digite o nome do arquivo de entrada: (sem \".txt\")" << '\n';

    std::cin >> input_file_name;
  }
  else
  {
    std::cout << "Digite o caminho completo do arquivo de entrada: (sem \".txt\")" << '\n';

    std::cin >> input_file_name;
  }

  read_file.open(input_file_name + ".txt");
  output_file.open("output.txt");

  char next_char;
  proximo(next_char);

  try
  {
    do
    {
      write_output(lexical_analysis(next_char));
    } while (read_file.good());
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  read_file.close();
  output_file.close();

  line_number = 1;
  column_number = 1;

  read_file.open("output.txt");

  try
  {
    fillQueueWithTokens();
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  read_file.close();

  // Pilha de tokens vai estar preenchida com os tokens do arquivo
  // Agora, vai ser feita a análise sintática
  sintaticAnalysis();

  printf("\n\nAnálise sintática concluída!\n");
  getchar();

  return 0;
}