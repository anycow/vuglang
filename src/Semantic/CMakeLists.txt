target_sources(Vug PRIVATE
        Symbol.hpp
        SymbolContext.cpp
        SymbolContext.hpp
        SymbolTable.cpp
        SymbolTable.hpp
        Type.cpp
        Type.hpp

        Passes/ModuleDefinitionPass.cpp
        Passes/ModuleDefinitionPass.hpp
        Passes/GlobalScopePass.cpp
        Passes/GlobalScopePass.hpp
        Passes/LocalScopePass.cpp
        Passes/LocalScopePass.hpp)