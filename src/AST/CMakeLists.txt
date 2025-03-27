target_sources(Vug PRIVATE
        ASTNodes.hpp
        ASTNodesForward.hpp
        ASTWalker.hpp

        Nodes/Node.hpp
        Nodes/Statement.hpp
        Nodes/Expression.hpp
        Nodes/Declaration.hpp

        Nodes/Declarations/DeclarationsBlock.hpp
        Nodes/Declarations/FunctionDeclaration.hpp
        Nodes/Declarations/FunctionParameter.hpp
        Nodes/Declarations/ModuleDeclaration.hpp

        Nodes/Expressions/BinaryOperation.hpp
        Nodes/Expressions/CallFunction.hpp
        Nodes/Expressions/Identifier.hpp
        Nodes/Expressions/Number.hpp
        Nodes/Expressions/PrefixOperation.hpp

        Nodes/Statements/Assign.hpp
        Nodes/Statements/Break.hpp
        Nodes/Statements/If.hpp
        Nodes/Statements/Print.hpp
        Nodes/Statements/Return.hpp
        Nodes/Statements/StatementsBlock.hpp
        Nodes/Statements/LocalVariableDeclaration.hpp
        Nodes/Statements/While.hpp
)