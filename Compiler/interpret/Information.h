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

    Information( common::Register r, ast::TypeOf type ) :
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

    void clear() {
        _state = State::InfoOnly;
        _break = false;
        _continue = false;
        _return = false;
    }

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

    const ast::TypeOf &type() const {
        return _variable.type();
    }

    void remember( common::Register r ) {
        _state = State::Constant;
        _value = r;
    }

    common::Register load() const {
        if ( rValue() )
            return _value;

        if ( type().kind() == ast::TypeOf::Kind::Pointer ) {
            int bytes = type().of()->bytes();
            bool sign = type().of()->isSigned();
            return common::Register( variable().as< void * >(), bytes, sign );
        }

        if ( type().kind() == ast::TypeOf::Kind::Array ) {
            int bytes = type().of()->bytes();
            bool sign = type().of()->isSigned();
            return common::Register( variable().address(), bytes, sign );
        }

        switch ( type().bytes() ) {
        case 1:
            if ( type().isSigned() )
                return common::Register( variable().as< int8_t >() );
            return common::Register( variable().as< uint8_t >() );
        case 2:
            if ( type().isSigned() )
                return common::Register( variable().as< int16_t >() );
            return common::Register( variable().as< uint16_t >() );
        case 4:
            if ( type().isSigned() )
                return common::Register( variable().as< int32_t >() );
            return common::Register( variable().as< uint32_t >() );
        case 8:
            if ( type().isSigned() )
                return common::Register( variable().as< int64_t >() );
            return common::Register( variable().as< uint64_t >() );
        default:
            throw std::runtime_error( "invalid type length" );
        }
    }

    void store( common::Register r ) {
        if ( !lValue() )
            throw exception::InternalError( "not l-value" );

        if ( type().kind() == ast::TypeOf::Kind::Pointer ) {
            variable().as< void * >() = r.getPtr();
            return;
        }

        if ( type().kind() == ast::TypeOf::Kind::Array )
            throw exception::InternalError( "not l-value" );


        switch ( type().bytes() ) {
        case 1:
            if ( type().isSigned() )
                variable().as< int8_t >() = r.get8();
            else
                variable().as< uint8_t >() = r.getu8();
            break;
        case 2:
            if ( type().isSigned() )
                variable().as< int16_t >() = r.get16();
            else
                variable().as< uint16_t >() = r.getu16();
            break;
        case 4:
            if ( type().isSigned() )
                variable().as< int32_t >() = r.get32();
            else
                variable().as< uint32_t >() = r.getu32();
            break;
        case 8:
            if ( type().isSigned() )
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
