#pragma once

#include "Variable.h"
#include "../common/Register.h"

namespace compiler {
namespace interpret {

struct Information {

    enum class State {
        Constant,
        Variable,
        InfoOnly
    };

    Information( common::Register r, const ast::type::Type *type ) :
        _state( State::Constant ),
        _value( r ),
        _variable( nullptr, type )
    {}

    Information( Variable v ) :
        _state( State::Variable ),
        _variable( v )
    {}

    Information() :
        _state( State::InfoOnly )
    {}

    bool rValue() const {
        return _state == State::Constant;
    }

    bool lValue() const {
        return _state == State::Variable;
    }

    bool infoOnly() const {
        return _state == State::InfoOnly;
    }

    common::Register value() const {
        return _value;
    }

    bool returning() const {
        return _return;
    }
    bool breaking() const {
        return _break;
    }
    bool continuing() const {
        return _continue;
    }

    bool skipping() const {
        return returning() || breaking() || continuing();
    }

    void returning( bool v ) {
        _return = v;
    }
    void breaking( bool v ) {
        _break = v;
    }
    void continuing( bool v ) {
        _continue = v;
    }


    Variable variable() const {
        return _variable;
    }

    Variable &variable() {
        return _variable;
    }

    const ast::type::Type *type() const {
        if ( lValue() )
            return _variable.type();
        if ( rValue() )
            return _variable.type();
        return nullptr;
    }

    void remember( common::Register r ) {
        _state = State::Constant;
        _value = r;
    }

    common::Register load() const {
        if ( rValue() )
            return _value;

        if ( type()->kind() == ast::type::Kind::Pointer ) {
            const auto &tOf = type()->as< ast::type::Pointer >()->of();
            bool sign = tOf.kind() == ast::type::Kind::Elementary ?
                tOf.as< ast::type::Elementary >()->isSigned() :
                false;
            return common::Register( variable().as< void * >(), tOf.size(), sign );
        }

        if ( type()->kind() == ast::type::Kind::Array ) {
            const auto &tOf = type()->as< ast::type::Array >()->of();
            bool sign = tOf.kind() == ast::type::Kind::Elementary ?
                tOf.as< ast::type::Elementary >()->isSigned() :
                false;
            return common::Register( variable().address(), tOf.size(), sign );
        }

        const ast::type::Elementary *t = type()->as< ast::type::Elementary >();

        switch ( t->length() ) {
        case 1:
            if ( t->isSigned() )
                return common::Register( variable().as< int8_t >() );
            return common::Register( variable().as< uint8_t >() );
        case 2:
            if ( t->isSigned() )
                return common::Register( variable().as< int16_t >() );
            return common::Register( variable().as< uint16_t >() );
        case 4:
            if ( t->isSigned() )
                return common::Register( variable().as< int32_t >() );
            return common::Register( variable().as< uint32_t >() );
        case 8:
            if ( t->isSigned() )
                return common::Register( variable().as< int64_t >() );
            return common::Register( variable().as< uint64_t >() );
        default:
            throw std::runtime_error( "invalid type length" );
        }
    }

    void store( common::Register r ) {
        if ( !lValue() )
            throw exception::InternalError( "not l-value" );

        if ( type()->kind() == ast::type::Kind::Pointer ) {
            variable().as< void * >() = r.getPtr();
            return;
        }

        if ( type()->kind() == ast::type::Kind::Array )
            throw exception::InternalError( "not l-value" );

        const ast::type::Elementary *t = type()->as< ast::type::Elementary >();

        switch ( t->length() ) {
        case 1:
            if ( t->isSigned() )
                variable().as< int8_t >() = r.get8();
            else
                variable().as< uint8_t >() = r.getu8();
            break;
        case 2:
            if ( t->isSigned() )
                variable().as< int16_t >() = r.get16();
            else
                variable().as< uint16_t >() = r.getu16();
            break;
        case 4:
            if ( t->isSigned() )
                variable().as< int32_t >() = r.get32();
            else
                variable().as< uint32_t >() = r.getu32();
            break;
        case 8:
            if ( t->isSigned() )
                variable().as< int64_t >() = r.get64();
            else
                variable().as< uint64_t >() = r.getu64();
            break;
        }
    }

private:
    State _state;
    bool _break = false;
    bool _continue = false;
    bool _return = false;
    Variable _variable;
    common::Register _value;
};

} // namespace interpret
} // namespace compiler
