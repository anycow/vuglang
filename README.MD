# Vuglang

Vuglang is a small toy programming language whose purpose is demonstration basic interpretation/compilation technics.

## Realisation

Vuglang implement a compiler interpreter in 6 stages:

1) Lexer (source &rarr; tokens)
2) [LL(1)](https://en.wikipedia.org/wiki/LL_parser) parsing (tokens &rarr; [AST](https://en.wikipedia.org/wiki/AST))
3) Module Definition Pass (declare all module and module members)
4) Global Scope Pass (define all symbols)
5) Local Scope Pass (process type semantic in function)
6) Evaluator (walk on attributed AST and make computation)

## TODO

- [x] Implement interpreter (evaluator)
- [x] Implement semi-advanced declaration processing (50/50 yet)
- [ ] Compilation errors
- [ ] Document code and language
- [ ] Implement semi-advanced type system (arrays, references, pointers, const, casts, structs)
- [ ] Implement stack machine
- [ ] Implement own register machine and bytecode
- [ ] Implement translation of own register machine IR to LLVM IR