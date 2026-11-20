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
#include "ps.file.h"
#include "ps.log.h"

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

typedef struct ps_ocs_field_t {
    ps_str_t name;
    ps_str_t value;
    ps_str_t preamble;
    ps_str_t postamble;
} ps_ocs_field_t;

typedef _ps_list_t(ps_ocs_field_t) ps_ocs_field_list_t;

typedef struct ps_ocs_parser_context_t {
    ps_str_t file_name;
    ps_str_t source;
    ps_ocs_token_list_t tokens;
    ps_half_t cursor;
} ps_ocs_parser_context_t;

typedef _ps_list_t(ps_ocs_parser_context_t) ps_ocs_parser_context_list_t;

typedef struct ps_ocs_parser_t {
    ps_ocs_parser_context_list_t context_stack;
    ps_ocs_field_list_t fields;
} ps_ocs_parser_t;

typedef ps_str_t (*ps_ocs_render_fn)(void* context, ps_ocs_field_list_t fields);

#define PS_OCS_KEYWORD_COUNT 1
PS_DEF const char* PS_OCS_KEYWORDS[PS_OCS_KEYWORD_COUNT];
PS_DEF const ps_ocs_token_tag_e PS_OCS_KEYWORD_TAGS[PS_OCS_KEYWORD_COUNT];
PS_DEF const char* PS_OCS_WHITESPACE_CHARS;
PS_DEF const char* PS_OCS_IDENTIFIER_CHARS;

PS_DEF ps_ocs_render_fn ps_ocs_render;

PS_DEF const char* ps_ocs_token_tag_to_string(ps_ocs_token_tag_e tag);
PS_DEF ps_str_view_t ps_ocs_token_to_string_view(ps_ocs_token_t token, ps_str_view_t source);
PS_DEF ps_ocs_token_t ps_ocs_lexer_next(ps_ocs_lexer_t *lexer);
PS_DEF ps_ocs_token_list_t ps_ocs_lexer_tokenize(ps_str_view_t source);

PS_DEF int ps_ocs_parser_add_file(ps_ocs_parser_t *parser, const char *file_name);
PS_DEF int ps_ocs_parser_add_source(ps_ocs_parser_t *parser, const char* psudo_file_name, const char* source);
PS_DEF int ps_ocs_parser_parse(ps_ocs_parser_t *parser);
PS_DEF int ps_ocs_parser_parse_using(ps_ocs_parser_t *parser, ps_ocs_parser_context_t *context);
PS_DEF int ps_ocs_parser_parse_dot_lbrace(ps_ocs_parser_t *parser, ps_ocs_parser_context_t *context);
PS_DEF int ps_ocs_parser_parse_lbrace(ps_ocs_parser_t *parser, ps_ocs_parser_context_t *context);
PS_DEF ps_half_t ps_ocs_parser_find_field(ps_ocs_parser_t *parser, const char* field_name);
PS_DEF int ps_ocs_context_expect_token(ps_ocs_parser_context_t *context, ps_ocs_token_tag_e tag);
PS_DEF int ps_ocs_context_is_token(ps_ocs_parser_context_t *context, ps_ocs_token_tag_e tag);
PS_DEF int __i_ps_ocs_context_is_any_token(ps_ocs_parser_context_t *context, ps_ocs_token_tag_e *tags, ps_half_t tags_size);
PS_DEF ps_str_t ps_ocs_context_render_span(ps_ocs_parser_context_t *context, ps_ocs_span_t span);
PS_DEF ps_str_t ps_ocs_context_render_token(ps_ocs_parser_context_t *context, ps_half_t token_index);
PS_DEF void ps_ocs_parser_free(ps_ocs_parser_t *parser);
PS_DEF void ps_ocs_parser_context_free(ps_ocs_parser_context_t *context);
PS_DEF void ps_ocs_field_free(ps_ocs_field_t *field);

#define _ps_ocs_context_is_any_token(context, ...) __i_ps_ocs_context_is_any_token(context, (ps_ocs_token_tag_e[]){__VA_ARGS__}, _ps_array_size(((ps_ocs_token_tag_e[]){__VA_ARGS__})))
#define _ps_ocs_context_is_skipable(context) _ps_ocs_context_is_any_token(context, PS_OCS_TOKEN_TAG_WHITESPACE, PS_OCS_TOKEN_TAG_MULTI_LINE_COMMENT, PS_OCS_TOKEN_TAG_SINGLE_LINE_COMMENT)
#define _ps_ocs_context_skip(context)\
    _ps_blk(\
        while (_ps_ocs_context_is_skipable(context)) context->cursor++;\
    )

PS_DEF ps_str_t ps_ocs_render_default(void *context, ps_ocs_field_list_t fields);

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

PS_DEF ps_ocs_render_fn ps_ocs_render = ps_ocs_render_default;

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
            if (strchr(PS_OCS_WHITESPACE_CHARS, c)){
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
            if (strchr(PS_OCS_WHITESPACE_CHARS, c)){
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
            if (strchr(PS_OCS_IDENTIFIER_CHARS, c)){
                _ps_ocs_lexer_step(lexer);
                if (lexer->cursor < lexer->source.size) goto ps_ocs_lexer_state_l;
            }
            token.tag = PS_OCS_TOKEN_TAG_IDENTIFIER;
            token.span.end = lexer->cursor;
            break;
        }
        case PS_OCS_LEXER_STATE_NAMED_IDENTIFIER:{
            if (strchr(PS_OCS_IDENTIFIER_CHARS, c)){
                _ps_ocs_lexer_step(lexer);
                if (lexer->cursor < lexer->source.size) goto ps_ocs_lexer_state_l;
            }
            token.tag = PS_OCS_TOKEN_TAG_NAMED_IDENTIFIER;
            token.span.end = lexer->cursor;
            break;
        }
        case PS_OCS_LEXER_STATE_KEYWORD:{
            if (strchr(PS_OCS_IDENTIFIER_CHARS, c)){
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
    if (lexer.cursor < source.size && source.items[lexer.cursor] != '\0'){
        ps_log_print(PS_LOG_ERROR, "Got Invalid token before end of lexer source. %d:%d\n", lexer.location.line, lexer.location.column);
        _ps_list_free((ps_ocs_token_t **)&tokens);
    }
    return tokens;
}

PS_DEF int ps_ocs_parser_add_file(ps_ocs_parser_t *parser, const char *file_name){
    if (!file_name){
        ps_log_print(PS_LOG_ERROR, "%s: parameter file_name was NULL\n", __func__);
        return 1;
    }
    char *dot = strrchr(file_name, '.');
    if(!dot || strcmp(dot, ".ocs")){
        ps_log_print(PS_LOG_ERROR, "%s: file_name extension is not .ocs\n", __func__);
        return 1;
    }
    if (!ps_file_exists(file_name)){
        ps_log_print(PS_LOG_ERROR, "%s: file %s does not exist\n", __func__, file_name);
        return 1;
    }

    ps_half_t file_name_len = strlen(file_name);
    _ps_list_foreach((ps_ocs_parser_context_t **)&parser->context_stack, context){
        if(file_name_len == context->file_name.size && ps_mem_eql(context->file_name.items, file_name, file_name_len)){
            return 0;
        }
    }
    ps_ocs_parser_context_t new_context = {0};
    _ps_str_concatz((char **)&new_context.file_name, file_name);
    new_context.source = ps_file_read_all(file_name);
    ps_str_view_t source_view = (ps_str_view_t){new_context.source.items, new_context.source.size};
    new_context.tokens = ps_ocs_lexer_tokenize(source_view);
    if (_ps_list_is_empty((ps_ocs_token_t **)&new_context.tokens) && !_ps_list_is_empty((char **)&new_context.source)){
        _ps_list_free((char **)&new_context.file_name);
        _ps_list_free((char **)&new_context.source);
        ps_log_print(PS_LOG_ERROR, "%s: unable to lex file %s\n", __func__, file_name);
        return 1;
    }
    _ps_list_append_one((ps_ocs_parser_context_t **)&parser->context_stack,new_context);
    return 0;
}

PS_DEF int ps_ocs_parser_add_source(ps_ocs_parser_t *parser, const char* psudo_file_name, const char* source){
    if (!psudo_file_name){
        ps_log_print(PS_LOG_ERROR, "%s: parameter psudo_file_name was NULL\n", __func__);
        return 1;
    }
    if (!source){
        return 0;
    }

    ps_half_t psudo_file_name_len = strlen(psudo_file_name);
    _ps_list_foreach((ps_ocs_parser_context_t **)&parser->context_stack, context){
        if(psudo_file_name_len == context->file_name.size && ps_mem_eql(context->file_name.items, psudo_file_name, psudo_file_name_len)){
            return 0;
        }
    }
    ps_ocs_parser_context_t new_context = {0};
    _ps_str_concatz((char **)&new_context.file_name, psudo_file_name);
    _ps_str_concatz((char **)&new_context.source, source);
    ps_str_view_t source_view = (ps_str_view_t){new_context.source.items, new_context.source.size};
    new_context.tokens = ps_ocs_lexer_tokenize(source_view);
    if (_ps_list_is_empty((ps_ocs_token_t **)&new_context.tokens) && !_ps_list_is_empty((char **)&new_context.source)){
        _ps_list_free((char **)&new_context.file_name);
        _ps_list_free((char **)&new_context.source);
        ps_log_print(PS_LOG_ERROR, "%s: unable to lex file %s\n", __func__, psudo_file_name);
        return 1;
    }
    _ps_list_append_one((ps_ocs_parser_context_t **)&parser->context_stack,new_context);
    return 0;
}

PS_DEF int ps_ocs_parser_parse(ps_ocs_parser_t *parser){
    if (parser->context_stack.size <= 0){
        return 0;
    }
    while (parser->context_stack.size > 0){
        ps_ocs_parser_context_t *context = &_ps_list_last((ps_ocs_parser_context_t **)&parser->context_stack);
        if (context->cursor >= context->tokens.size){
            ps_ocs_parser_context_free(context);
            _ps_unused(_ps_list_pop((ps_ocs_parser_context_t **)&parser->context_stack));
            continue;
        }

        ps_ocs_token_t token = _ps_list_get((ps_ocs_token_t **)&context->tokens, context->cursor);
        switch (token.tag){
            case PS_OCS_TOKEN_TAG_KEYWORD_USING:
                if(ps_ocs_parser_parse_using(parser, context)){
                    goto exit_error_l;
                }
                continue;
            case PS_OCS_TOKEN_TAG_DOT_LBRACE:
                if(ps_ocs_parser_parse_dot_lbrace(parser, context)){
                    goto exit_error_l;
                }
                break;
            case PS_OCS_TOKEN_TAG_LBRACE:
                if(ps_ocs_parser_parse_lbrace(parser, context)){
                    goto exit_error_l;
                }
                break;
            default:
                break;
            
        }
        context->cursor++;
    }
    _ps_list_foreach((ps_ocs_field_t **)&parser->fields, field){
        if (_ps_list_is_empty((char **)&field->value) || field->value.size == 1){
            ps_log_print(PS_LOG_ERROR, "%s: field '%s' has no value\n", __func__, field->name.items);
            goto exit_error_l;
        }
    }
    goto exit_l;
    exit_error_l:
        ps_ocs_parser_free(parser);
        return 1;
    exit_l:
        _ps_list_foreach((ps_ocs_parser_context_t **)&parser->context_stack, context){
            ps_ocs_parser_context_free(context);
        }
        _ps_list_free((ps_ocs_parser_context_t **)&parser->context_stack);
        return 0;
}

PS_DEF int ps_ocs_parser_parse_using(ps_ocs_parser_t *parser, ps_ocs_parser_context_t *context){
    if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_KEYWORD_USING)) return 1;
    _ps_ocs_context_skip(context);
    ps_half_t file_str_index = context->cursor;
    if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_STRING)) return 1;
    ps_ocs_token_t file_token = _ps_list_get((ps_ocs_token_t **)&context->tokens, file_str_index);
    ps_str_view_t source_view = (ps_str_view_t){context->source.items, context->source.size};
    ps_str_view_t file_token_view = ps_ocs_token_to_string_view(file_token, source_view);
    _ps_slice_skip((char **)&file_token_view,1);
    _ps_slice_shrink((char **)&file_token_view,1);
    ps_str_t file_name = {0};
    _ps_list_append_many((char **)&file_name, file_token_view.items, file_token_view.size);
    _ps_list_append_one((char **)&file_name, '\0');
    // NOTE: maybe ps_ocs_parser_add_file should accept a ps_str_t, but this makes it so it ops with non ps.h stuff better
    int err = ps_ocs_parser_add_file(parser, file_name.items);
    _ps_list_free((char **)&file_name);
    return err;
}
PS_DEF int ps_ocs_parser_parse_dot_lbrace(ps_ocs_parser_t *parser, ps_ocs_parser_context_t *context){
    if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_DOT_LBRACE)) return 1;
    _ps_ocs_context_skip(context);
    while (!ps_ocs_context_is_token(context, PS_OCS_TOKEN_TAG_RBRACE) && context->cursor < context->tokens.size){
        ps_half_t named_identifier_token_index = context->cursor;
        if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_NAMED_IDENTIFIER)) return 1;
        ps_str_t field_name = ps_ocs_context_render_token(context, named_identifier_token_index);
        ps_half_t field_index = ps_ocs_parser_find_field(parser, field_name.items);
        if (field_index == -1){
            ps_ocs_token_t named_identifier_token = _ps_list_get((ps_ocs_token_t **)&context->tokens, named_identifier_token_index);
            ps_log_print(PS_LOG_ERROR, "%s: Unknown field name: %s at %s %d:%d\n", __func__, field_name.items, context->file_name.items, named_identifier_token.location.line, named_identifier_token.location.column);
            _ps_list_free((char **)&field_name);
            return 1;
        }
        _ps_ocs_context_skip(context);
        if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_EQUALS)) return 1;
        _ps_ocs_context_skip(context);
        ps_ocs_span_t value_tokens_span = {0};
        value_tokens_span.start = context->cursor;
        while (!ps_ocs_context_is_token(context, PS_OCS_TOKEN_TAG_SEMICOLON) && context->cursor < context->tokens.size) context->cursor++;
        value_tokens_span.end = context->cursor;
        if(ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_SEMICOLON)) return 1;
        _ps_ocs_context_skip(context);

        ps_str_t field_value = ps_ocs_context_render_span(context, value_tokens_span);
        ps_ocs_field_t *field = &_ps_list_get((ps_ocs_field_t **)&parser->fields, field_index);
        if(!_ps_list_is_empty((char **)&field->value)){
            _ps_list_free((char **)&field->value);
        }
        field->value = field_value;
    }
    if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_RBRACE)) return 1;
    return 0;
}
PS_DEF int ps_ocs_parser_parse_lbrace(ps_ocs_parser_t *parser, ps_ocs_parser_context_t *context){
    if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_LBRACE)) return 1;
    _ps_ocs_context_skip(context);
    while (!ps_ocs_context_is_token(context, PS_OCS_TOKEN_TAG_RBRACE) && context->cursor < context->tokens.size){
        ps_ocs_span_t field_preamble_span = {0};
        field_preamble_span.start = context->cursor;
        while(context->cursor < context->tokens.size && !ps_ocs_context_is_token(context, PS_OCS_TOKEN_TAG_NAMED_IDENTIFIER)) context->cursor++;
        field_preamble_span.end = context->cursor;

        ps_half_t field_name_token_index = context->cursor;
        if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_NAMED_IDENTIFIER)) return 1;
        ps_ocs_span_t field_postamble_span = {0};
        field_postamble_span.start = context->cursor;
        while(context->cursor < context->tokens.size && !ps_ocs_context_is_token(context, PS_OCS_TOKEN_TAG_SEMICOLON)) context->cursor++;
        field_postamble_span.end = context->cursor;
        if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_SEMICOLON)) return 1;
        _ps_ocs_context_skip(context);

        ps_str_t field_name = ps_ocs_context_render_token(context, field_name_token_index);
        ps_half_t field_index = ps_ocs_parser_find_field(parser,field_name.items);
        if (field_index != -1){
            ps_ocs_token_t field_name_token = _ps_list_get((ps_ocs_token_t **)&context->tokens, field_name_token_index);
            ps_log_print(PS_LOG_ERROR, "%s: Duplicate field name %s, at %s %d:%d\n", __func__, field_name.items, context->file_name.items, field_name_token.location.line, field_name_token.location.column);
            _ps_list_free((char **)&field_name);
            return 1;
        }
        ps_ocs_field_t field = {0};
        field.name = field_name;
        field.preamble = ps_ocs_context_render_span(context, field_preamble_span);
        field.postamble = ps_ocs_context_render_span(context, field_postamble_span);
        _ps_list_append_one((ps_ocs_field_t **)&parser->fields, field);
    }
    if (ps_ocs_context_expect_token(context, PS_OCS_TOKEN_TAG_RBRACE)) return 1;
    return 0;
}


PS_DEF ps_half_t ps_ocs_parser_find_field(ps_ocs_parser_t *parser, const char* field_name){
    ps_half_t i = 0;
    _ps_list_foreach((ps_ocs_field_t **)&parser->fields, field){
        if(!strcmp(field->name.items, field_name)){
            return i;
        }
        i++;
    }
    return -1;
}

PS_DEF int ps_ocs_context_expect_token(ps_ocs_parser_context_t *context, ps_ocs_token_tag_e tag){
    if (context->cursor >= context->tokens.size){
        ps_log_print(PS_LOG_ERROR, "%s: Expected token tag: %s, (Out of bounds)\n", __func__, ps_ocs_token_tag_to_string(tag));
        return 1;
    }
    ps_ocs_token_t token = _ps_list_get((ps_ocs_token_t **)&context->tokens, context->cursor);
    if (token.tag != tag){
        ps_log_print(PS_LOG_ERROR, "%s: Expected token tag: %s, got %s at %s %d:%d\n", __func__, ps_ocs_token_tag_to_string(tag), ps_ocs_token_tag_to_string(token.tag), context->file_name.items, token.location.line, token.location.column);
        return 1;
    }
    context->cursor++;
    return 0;
}

PS_DEF int ps_ocs_context_is_token(ps_ocs_parser_context_t *context, ps_ocs_token_tag_e tag){
    if (context->cursor >= context->tokens.size){
        return 0;
    }
    ps_ocs_token_t token = _ps_list_get((ps_ocs_token_t **)&context->tokens, context->cursor);
    return token.tag == tag;
}

PS_DEF int __i_ps_ocs_context_is_any_token(ps_ocs_parser_context_t *context, ps_ocs_token_tag_e *tags, ps_half_t tags_size){
    ps_half_t i = 0;
    while(i < tags_size){
        if (ps_ocs_context_is_token(context,tags[i])) return 1;
        i++;
    }
    return 0;
}

PS_DEF ps_str_t ps_ocs_context_render_span(ps_ocs_parser_context_t *context, ps_ocs_span_t span){
    ps_str_t result = {0};
    for (ps_half_t i = span.start; i < span.end; ++i){
        ps_ocs_token_t token = _ps_list_get((ps_ocs_token_t **)&context->tokens, i);
        ps_str_view_t source_view = (ps_str_view_t){context->source.items, context->source.size};
        ps_str_view_t token_view = ps_ocs_token_to_string_view(token, source_view);
        _ps_list_append_many((char **)&result, token_view.items, token_view.size);
    }
    _ps_list_append_one((char**)&result, '\0');
    return result;
}
PS_DEF ps_str_t ps_ocs_context_render_token(ps_ocs_parser_context_t *context, ps_half_t token_index){
    ps_str_t result = {0};
    ps_ocs_token_t token = _ps_list_get((ps_ocs_token_t **)&context->tokens, token_index);
    ps_str_view_t source_view = (ps_str_view_t){context->source.items, context->source.size};
    ps_str_view_t token_view = ps_ocs_token_to_string_view(token,source_view);
    _ps_list_append_many((char **)&result, token_view.items, token_view.size);
    _ps_list_append_one((char **)&result, '\0');
    return result;
}

PS_DEF void ps_ocs_parser_free(ps_ocs_parser_t *parser){
    _ps_list_foreach((ps_ocs_parser_context_t **)&parser->context_stack, context){
        ps_ocs_parser_context_free(context);
    }
    _ps_list_free((ps_ocs_parser_context_t **)&parser->context_stack);
    _ps_list_foreach((ps_ocs_field_t **)&parser->fields, field){
        ps_ocs_field_free(field);
    }
    _ps_list_free((ps_ocs_field_t **)&parser->fields);
}
PS_DEF void ps_ocs_parser_context_free(ps_ocs_parser_context_t *context){
    _ps_list_free((char **)&context->file_name);
    _ps_list_free((char **)&context->source);
    _ps_list_free((ps_ocs_token_t **)&context->tokens);
    context->cursor = 0;
}
PS_DEF void ps_ocs_field_free(ps_ocs_field_t *field){
    _ps_list_free((char **)&field->name);
    _ps_list_free((char **)&field->value);
    _ps_list_free((char **)&field->preamble);
    _ps_list_free((char **)&field->postamble);
}

PS_DEF ps_str_t ps_ocs_render_default(void* context, ps_ocs_field_list_t fields){
    // default renderer assums context is a char** aka ps_str_t*
    ps_str_t result = {0};
    if (!context || !*(char **)context){
        ps_log_print(PS_LOG_ERROR, "%s: no ouput file name provided to renderer\n", __func__);
        return result;
    }
    if (_ps_list_is_empty((ps_ocs_field_t **)&fields)){
        return result;
    }
    char **output_file_name = context;
    char *slash = strrchr(*output_file_name, PS_PATH_SEP);
    if(!slash){
        slash = strrchr(*output_file_name, PS_ALT_PATH_SEP);
        if(!slash){
            slash = *output_file_name;
        }
    }
    if (slash != *output_file_name) slash++;
    char *dot = strrchr(slash, '.');
    ps_half_t name_len = dot ? dot - slash : strlen(slash);
    ps_str_t lower_name = {0};
    ps_str_t upper_name = {0};
    _ps_list_reserve_precise((char**)&lower_name, name_len + 1);
    lower_name.size = name_len + 1;
    _ps_list_reserve_precise((char**)&upper_name, name_len + 1);
    upper_name.size = name_len + 1;
    for (ps_half_t i = 0; i < name_len; i++){
        char c = slash[i];
        _ps_list_get((char **)&lower_name, i) = tolower(c);
        _ps_list_get((char **)&upper_name, i) = toupper(c);
        if(strchr(" .-",c)){
            _ps_list_set((char **)&lower_name, i, '_');
            _ps_list_set((char **)&upper_name, i, '_');
        }
    }
    _ps_list_set((char **)&lower_name, name_len, '\0');
    _ps_list_set((char **)&upper_name, name_len, '\0');

    _ps_str_concat((char **)&result, "/*\n");
    _ps_str_concat((char **)&result, " * This file was generated using ps.ocs.h by David J Meagher.\n");
    _ps_str_concat((char **)&result, " * it is not recommended to edit this file manually.\n");
    _ps_str_concat((char **)&result, "*/\n");

    _ps_str_concat((char **)&result, "#ifndef _H_", upper_name.items, "_H_\n");
    _ps_str_concat((char **)&result, "#define _H_", upper_name.items, "_H_\n\n");

    _ps_str_concat((char **)&result, "#ifdef OCS_AS_MACRO\n");
    _ps_list_foreach((ps_ocs_field_t **)&fields, field){
        ps_str_t upper_field_name = {0};
        _ps_list_reserve_precise((char **)&upper_field_name, field->name.size);
        upper_field_name.size = field->name.size;
        ps_half_t i = 0;
        _ps_list_foreach((char **)&field->name, name){
            _ps_list_get((char **)&upper_field_name,i) = toupper(*name);
            i++;
        }
        _ps_str_concat((char **)&result, "#define ", upper_name.items, "_", upper_field_name.items + 1, " ", field->value.items, "\n");
        _ps_list_free((char **)&upper_field_name);
    }
    _ps_str_concat((char **)&result, "#endif //OCS_AS_MACRO\n\n");
    
    _ps_str_concat((char **)&result, "typedef struct ", lower_name.items, "_t {\n");
    _ps_list_foreach((ps_ocs_field_t **)&fields, field){
        _ps_str_concat((char **)&result, "    ", field->preamble.items, field->name.items + 1, field->postamble.items, ";\n");
    }
    _ps_str_concat((char **)&result, "} ", lower_name.items, "_t;\n\n");

    _ps_str_concat((char **)&result, "#define ", upper_name.items, "_DEFAULT {\\\n");
    _ps_list_foreach((ps_ocs_field_t **)&fields, field){
        if (field < fields.items + fields.size - 1){
            _ps_str_concat((char **)&result, "    ", field->value.items, ",\\\n");
        } else {
            _ps_str_concat((char **)&result, "    ", field->value.items, "\\\n");
        }
    }
    _ps_str_concat((char **)&result, "}\n\n");
    _ps_str_concat((char **)&result, "#endif", " //_H_", upper_name.items, "_H_\n");

    _ps_list_free((char **)&upper_name);
    _ps_list_free((char **)&lower_name);
    return result;
}


#endif /* PS_IMP */

#endif /* _H_PS_OCS_H_ */