# Vug

Vug is a small toy programming language whose purpose is the demonstration of compilation techniques.

## Implementation

Vuglang implements a compiler (meaning explicitly building an attributed AST) in 8 stages:

1) Lexer (source &rarr; tokens)
2) [LL](https://en.wikipedia.org/wiki/LL_parser) parser (tokens &rarr; [AST](https://en.wikipedia.org/wiki/AST))
3) Module definition pass (declare all module and module members)
4) Global scope pass (define all symbols)
5) Local scope pass (process type semantic in functions and attribute AST nodes)
6) LLVM emitter (walks over attributed AST and emit)
7) LLVM optimization passes
8) LLVM compiler

## Example

```
mod fib_test {
	import func printNum(int32 n) -> int32;

	func fib(int32 n) -> int32 {
		if(n < 2) {
			return n;
		} else {
			return fib(n-1) + fib(n-2);		
		}
	}

	func main() -> int32 {
		return fib(100);
	}
}```

## TODO

- [x] Implement LLVM-based compilation
- [x] Implement semi-advanced declaration processing (50/50 yet)
- [x] Compilation errors
- [ ] Document code and language
- [ ] Implement semi-advanced type system (arrays, references, pointers, const, casts, structs)
