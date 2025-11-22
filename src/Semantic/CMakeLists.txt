target_sources(Vug PRIVATE
        Symbol.hpp
        SymbolContext.cpp
        SymbolContext.hpp
        SymbolTable.cpp
        SymbolTable.hpp

        Types/Type.cpp
        Types/Type.hpp
        Types/UndefinedType.cpp
        Types/UndefinedType.hpp
        Types/PrimitiveType.cpp
        Types/PrimitiveType.hpp
        Types/IntegerType.cpp
        Types/IntegerType.hpp
        Types/BooleanType.cpp
        Types/BooleanType.hpp

        Passes/ModuleDefinitionPass.cpp
        Passes/ModuleDefinitionPass.hpp
        Passes/GlobalScopePass.cpp
        Passes/GlobalScopePass.hpp
        Passes/LocalScopePass.cpp
        Passes/LocalScopePass.hpp)