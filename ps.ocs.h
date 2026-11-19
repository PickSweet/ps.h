/*
MIT License

Copyright (c) 2026 David J. Meagher

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#ifndef _H_PS_OCS_H_
#define _H_PS_OCS_H_

#include "ps.core.h"
#include "ps.collections.h"

typedef struct ps_ocs_location_t {
    ps_half_t line;
    ps_half_t column;
} ps_ocs_location_t;

typedef struct ps_ocs_span_t {
    ps_half_t start;
    ps_half_t end;
} ps_ocs_span_t;

typedef enum ps_ocs_token_tag_e {
    PS_OCS_TOKEN_TAG_INVALID = 0,
    PS_OCS_TOKEN_TAG_WHITESPACE,
    PS_OCS_TOKEN_TAG_SINGLE_LINE_COMMENT,
    PS_OCS_TOKEN_TAG_MULTI_LINE_COMMENT,
    PS_OCS_TOKEN_TAG_STRING,
    PS_OCS_TOKEN_TAG_IDENTIFIER,
    PS_OCS_TOKEN_TAG_NAMED_IDENTIFIER,
    PS_OCS_TOKEN_TAG_DOT_LBRACE,
    PS_OCS_TOKEN_TAG_LBRACE,
    PS_OCS_TOKEN_TAG_RBRACE,
    PS_OCS_TOKEN_TAG_LBRACKET,
    PS_OCS_TOKEN_TAG_RBRACKET,
    PS_OCS_TOKEN_TAG_SEMICOLON,
    PS_OCS_TOKEN_TAG_EQUALS,
    PS_OCS_TOKEN_TAG_KEYWORD_USING,
} ps_ocs_token_tag_e;

typedef struct ps_ocs_token_t {
    ps_ocs_token_tag_e tag;
    ps_ocs_span_t span;
    ps_ocs_location_t location;
} ps_ocs_token_t;

typedef enum ps_ocs_lexer_state_e {
    PS_OCS_LEXER_STATE_START = 0,
    PS_OCS_LEXER_STATE_WHITESPACE,
    PS_OCS_LEXER_STATE_SINGLE_LINE_COMMENT,
    PS_OCS_LEXER_STATE_MULTI_LINE_COMMENT,
    PS_OCS_LEXER_STATE_STRING,
    PS_OCS_LEXER_STATE_IDENTIFIER,
    PS_OCS_LEXER_STATE_NAMED_IDENTIFIER,
    PS_OCS_LEXER_STATE_KEYWORD,
} ps_ocs_lexer_state_e;

typedef struct ps_ocs_lexer_t {
    ps_str_view_t source;
    ps_half_t cursor;
    ps_ocs_location_t location;
} ps_ocs_lexer_t;

typedef _ps_list_t(ps_ocs_token_t) ps_ocs_token_list_t;
typedef _ps_slice_t(ps_ocs_token_t) ps_ocs_token_view_t;

typedef struct ps_ocs_field_t {
    ps_str_view_t name;
    ps_str_view_t value;
    ps_str_view_t preamble;
    ps_str_view_t postamble;
} ps_ocs_field_t;

typedef _ps_list_t(ps_ocs_field_t) ps_ocs_field_list_t;

typedef struct ps_ocs_parser_context_t {
    ps_str_view_t file_name;
    ps_str_view_t source;
    ps_ocs_token_view_t tokens;
    ps_half_t cursor;
} ps_ocs_parser_context_t;

typedef _ps_list_t(ps_ocs_parser_context_t) ps_ocs_parser_context_list_t;

typedef struct ps_ocs_parser_t {
    ps_str_t _temp_; // TODO: add this to the style guide for variables that are reserved for internal use in structures
    ps_ocs_parser_context_list_t context_stack;
    ps_ocs_field_list_t fields;
} ps_ocs_parser_t;

#define PS_OCS_KEYWORD_COUNT 1
PS_DEF const char* PS_OCS_KEYWORDS[PS_OCS_KEYWORD_COUNT];
PS_DEF const ps_ocs_token_tag_e PS_OCS_KEYWORD_TAGS[PS_OCS_KEYWORD_COUNT];
PS_DEF const char* PS_OCS_WHITESPACE_CHARS;
PS_DEF const char* PS_OCS_IDENTIFIER_CHARS;

PS_DEF const char* ps_ocs_token_tag_to_string(ps_ocs_token_tag_e tag);
PS_DEF ps_str_view_t ps_ocs_token_to_string_view(ps_ocs_token_t token, ps_str_view_t source);
PS_DEF ps_ocs_token_t ps_ocs_lexer_next(ps_ocs_lexer_t *lexer);
PS_DEF ps_ocs_token_list_t ps_ocs_lexer_tokenize(ps_str_view_t source);

PS_DEF int ps_ocs_parser_add_file(ps_ocs_parser_t *parser, const char *file_name);
PS_DEF int ps_ocs_parser_parse(ps_ocs_parser_t *parser);


#define PS_IMP

#ifdef PS_IMP

PS_DEF const char* PS_OCS_KEYWORDS[PS_OCS_KEYWORD_COUNT] = {
    "#using",
};

PS_DEF const ps_ocs_token_tag_e PS_OCS_KEYWORD_TAGS[PS_OCS_KEYWORD_COUNT] = {
    PS_OCS_TOKEN_TAG_KEYWORD_USING,
};

PS_DEF const char *PS_OCS_WHITESPACE_CHARS = " \r\n\t";
PS_DEF const char *PS_OCS_IDENTIFIER_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";


PS_DEF const char* ps_ocs_token_tag_to_string(ps_ocs_token_tag_e tag){
    switch(tag){
        case PS_OCS_TOKEN_TAG_INVALID: return "INVALID";
        case PS_OCS_TOKEN_TAG_KEYWORD_USING: return "KEYWORD_USING";
        case PS_OCS_TOKEN_TAG_IDENTIFIER: return "IDENTIFIER";
        case PS_OCS_TOKEN_TAG_NAMED_IDENTIFIER: return "NAMED_IDENTIFIER";
        case PS_OCS_TOKEN_TAG_WHITESPACE: return "WHITESPACE";
        case PS_OCS_TOKEN_TAG_STRING: return "STRING";
        case PS_OCS_TOKEN_TAG_SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case PS_OCS_TOKEN_TAG_MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case PS_OCS_TOKEN_TAG_DOT_LBRACE: return "DOT_LBRACE";
        case PS_OCS_TOKEN_TAG_LBRACE: return "LBRACE";
        case PS_OCS_TOKEN_TAG_RBRACE: return "RBRACE";
        case PS_OCS_TOKEN_TAG_LBRACKET: return "LBRACKET";
        case PS_OCS_TOKEN_TAG_RBRACKET: return "RBRACKET";
        case PS_OCS_TOKEN_TAG_SEMICOLON: return "SEMICOLON";
        case PS_OCS_TOKEN_TAG_EQUALS: return "EQUALS";
    }
    return "<UNKNOWN>";
}

PS_DEF ps_str_view_t ps_ocs_token_to_string_view(ps_ocs_token_t token, ps_str_view_t source){
    return (*(ps_str_view_t*)&_ps_slice_sub(&source.items, token.span.start, token.span.end));
}


PS_DEF void ps_ocs_lexer_advance(ps_ocs_lexer_t *lexer, ps_half_t i){
    lexer->cursor += i;
    lexer->location.column += i;
}

#define _ps_ocs_lexer_step(lexer) ps_ocs_lexer_advance((lexer), 1)

PS_DEF ps_ocs_token_t ps_ocs_lexer_next(ps_ocs_lexer_t *lexer){
    ps_ocs_token_t token = {
        PS_OCS_TOKEN_TAG_INVALID,
        {lexer->cursor, lexer->cursor},
        lexer->location
    };
    
    ps_ocs_lexer_state_e state = PS_OCS_LEXER_STATE_START;
    ps_ocs_lexer_state_l:
    
    if(lexer->cursor >= lexer->source.size || lexer->source.items[lexer->cursor] == '\0'){
        token.span.end = lexer->cursor;
        return token;
    }
    const char c = lexer->source.items[lexer->cursor];
    switch (state){
        case PS_OCS_LEXER_STATE_START:{
            if (c == '\0') break;
            if (strchr(PS_OCS_WHITESPACE_CHARS, c) != NULL){
                state = PS_OCS_LEXER_STATE_WHITESPACE;
                goto ps_ocs_lexer_state_l;
            }
            switch(c){
                case '/':{
                    if (lexer->cursor + 1 < lexer->source.size){
                        const char next_c = lexer->source.items[lexer->cursor + 1];
                        if (next_c == '/'){
                            state = PS_OCS_LEXER_STATE_SINGLE_LINE_COMMENT;
                            ps_ocs_lexer_advance(lexer, 2);
                            goto ps_ocs_lexer_state_l;
                        } else if (next_c == '*'){
                            state = PS_OCS_LEXER_STATE_MULTI_LINE_COMMENT;
                            ps_ocs_lexer_advance(lexer, 2);
                            goto ps_ocs_lexer_state_l;
                        }
                    }
                    break;
                }
                case '"':{
                    state = PS_OCS_LEXER_STATE_STRING;
                    _ps_ocs_lexer_step(lexer);
                    goto ps_ocs_lexer_state_l;
                }
                case '$':{
                    state = PS_OCS_LEXER_STATE_NAMED_IDENTIFIER;
                    _ps_ocs_lexer_step(lexer);
                    goto ps_ocs_lexer_state_l;
                }
                case '#':{
                    state = PS_OCS_LEXER_STATE_KEYWORD;
                    _ps_ocs_lexer_step(lexer);
                    goto ps_ocs_lexer_state_l;
                }
                case '.':{
                    if (lexer->cursor + 1 < lexer->source.size && lexer->source.items[lexer->cursor + 1] == '{'){
                        token.tag = PS_OCS_TOKEN_TAG_DOT_LBRACE;
                        ps_ocs_lexer_advance(lexer, 2);
                        token.span.end = lexer->cursor;
                        return token;
                    }
                    break;
                }
                case '{': token.tag = PS_OCS_TOKEN_TAG_LBRACE; break;
                case '}': token.tag = PS_OCS_TOKEN_TAG_RBRACE; break;
                case '[': token.tag = PS_OCS_TOKEN_TAG_LBRACKET; break;
                case ']': token.tag = PS_OCS_TOKEN_TAG_RBRACKET; break;
                case ';': token.tag = PS_OCS_TOKEN_TAG_SEMICOLON; break;
                case '=': token.tag = PS_OCS_TOKEN_TAG_EQUALS; break;
            }
            if (token.tag != PS_OCS_TOKEN_TAG_INVALID){
                _ps_ocs_lexer_step(lexer);
                token.span.end = lexer->cursor;
                return token;
            }

            state = PS_OCS_LEXER_STATE_IDENTIFIER;
            _ps_ocs_lexer_step(lexer);
            if (lexer->cursor < lexer->source.size) goto ps_ocs_lexer_state_l;
            token.tag = PS_OCS_TOKEN_TAG_IDENTIFIER;
            token.span.end = lexer->cursor;
            break;
        }
        case PS_OCS_LEXER_STATE_WHITESPACE:{
            if (strchr(PS_OCS_WHITESPACE_CHARS, c) != NULL){
                if (c == '\n'){
                    lexer->location.line += 1;
                    lexer->location.column = 0;
                }
                _ps_ocs_lexer_step(lexer);
                if (lexer->cursor < lexer->source.size) goto ps_ocs_lexer_state_l;
            }
            token.tag = PS_OCS_TOKEN_TAG_WHITESPACE;
            token.span.end = lexer->cursor;
            return token;
        }
        case PS_OCS_LEXER_STATE_SINGLE_LINE_COMMENT:{
            if (c == '\n'){
                token.tag = PS_OCS_TOKEN_TAG_SINGLE_LINE_COMMENT;
                token.span.end = lexer->cursor;
                return token;
            }
            _ps_ocs_lexer_step(lexer);
            if (lexer->cursor < lexer->source.size) goto ps_ocs_lexer_state_l;
            token.tag = PS_OCS_TOKEN_TAG_SINGLE_LINE_COMMENT;
            token.span.end = lexer->cursor;
            break;
        }
        case PS_OCS_LEXER_STATE_MULTI_LINE_COMMENT:{
            if (c == '\0'){
                token.tag = PS_OCS_TOKEN_TAG_INVALID;
                token.span.end = lexer->cursor;
                return token;
            }
            if (c == '\n'){
                lexer->location.line += 1;
                lexer->location.column = 0;
            }
            if (c == '*' && lexer->cursor + 1 < lexer->source.size && lexer->source.items[lexer->cursor + 1] == '/'){
                token.tag = PS_OCS_TOKEN_TAG_MULTI_LINE_COMMENT;
                ps_ocs_lexer_advance(lexer, 2);
                token.span.end = lexer->cursor;
                return token;
            }
            _ps_ocs_lexer_step(lexer);
            goto ps_ocs_lexer_state_l;
        }
        case PS_OCS_LEXER_STATE_STRING:{
            if (c == '\0'){
                token.tag = PS_OCS_TOKEN_TAG_INVALID;
                token.span.end = lexer->cursor;
                return token;
            }
            if (c == '\n'){
                lexer->location.line += 1;
                lexer->location.column = 0;
            }
            if (c == '\\' && lexer->cursor + 1 < lexer->source.size && lexer->source.items[lexer->cursor + 1] != '\0'){
                ps_ocs_lexer_advance(lexer, 2);
                goto ps_ocs_lexer_state_l;

            }
            if (c == '"'){
                token.tag = PS_OCS_TOKEN_TAG_STRING;
                _ps_ocs_lexer_step(lexer);
                token.span.end = lexer->cursor;
                return token;
            }
            _ps_ocs_lexer_step(lexer);
            goto ps_ocs_lexer_state_l;
        }
        case PS_OCS_LEXER_STATE_IDENTIFIER:{
            if (strchr(PS_OCS_IDENTIFIER_CHARS, c) != NULL){
                _ps_ocs_lexer_step(lexer);
                if (lexer->cursor < lexer->source.size) goto ps_ocs_lexer_state_l;
            }
            token.tag = PS_OCS_TOKEN_TAG_IDENTIFIER;
            token.span.end = lexer->cursor;
            break;
        }
        case PS_OCS_LEXER_STATE_NAMED_IDENTIFIER:{
            if (strchr(PS_OCS_IDENTIFIER_CHARS, c) != NULL){
                _ps_ocs_lexer_step(lexer);
                if (lexer->cursor < lexer->source.size) goto ps_ocs_lexer_state_l;
            }
            token.tag = PS_OCS_TOKEN_TAG_NAMED_IDENTIFIER;
            token.span.end = lexer->cursor;
            break;
        }
        case PS_OCS_LEXER_STATE_KEYWORD:{
            if (strchr(PS_OCS_IDENTIFIER_CHARS, c) != NULL){
                _ps_ocs_lexer_step(lexer);
                if (lexer->cursor < lexer->source.size) goto ps_ocs_lexer_state_l;
            }
            token.span.end = lexer->cursor;
            ps_str_view_t keyword_view = (*(ps_str_view_t*)&_ps_slice_sub(&lexer->source.items, token.span.start, lexer->cursor));
            int i = PS_OCS_KEYWORD_COUNT - 1;
            do {
                if (strlen(PS_OCS_KEYWORDS[i]) == keyword_view.size && ps_mem_eql(keyword_view.items, PS_OCS_KEYWORDS[i], keyword_view.size)){
                    token.tag = PS_OCS_KEYWORD_TAGS[i];
                    return token;
                }
            }while (i--);
            token.tag = PS_OCS_TOKEN_TAG_IDENTIFIER;
            break;
        }
    }
    
    return token;
}

PS_DEF ps_ocs_token_list_t ps_ocs_lexer_tokenize(ps_str_view_t source){
    ps_ocs_lexer_t lexer = {0};
    lexer.source = source;
    lexer.cursor = 0;
    lexer.location.line = 1;
    lexer.location.column = 1;

    ps_ocs_token_list_t tokens = {0};
    ps_ocs_token_t token = ps_ocs_lexer_next(&lexer);
    while (token.tag != PS_OCS_TOKEN_TAG_INVALID) {
        _ps_list_append_one(&tokens.items, token);
        token = ps_ocs_lexer_next(&lexer);
    }
    if (lexer.cursor < source.size && source.items[lexer.cursor] != '\0')
        ps_log_print(PS_LOG_ERROR, "Got Invalid token before end of lexer source. %d:%d", lexer.location.line, lexer.location.column);
    return tokens;
}

#endif /* PS_IMP */

#endif /* _H_PS_OCS_H_ */