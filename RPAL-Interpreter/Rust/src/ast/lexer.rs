use std::iter::Peekable;
use std::str::Chars;

pub enum TokenKind {
    IDENTIFIER(String),
    INTEGER(String),
    KEYWORD(String),
    OPERATOR(String),
    PUNCTUATION(String),
    STRING(String),
}

pub struct TextSpan {
    pub start: usize,
    pub end: usize,
    pub literal: String,
}

pub struct Token {
    pub span: TextSpan,
    pub kind: TokenKind,
}

pub struct Lexer<'a> {
    input: Peekable<Chars<'a>>,
}

impl TextSpan {
    pub fn new(start: usize, end: usize, literal: String) -> Self {
        Self {
            start,
            end,
            literal,
        }
    }

    pub fn length(&self) -> usize {
        self.end - self.start
    }
}

impl Token {
    pub fn new(span: TextSpan, kind: TokenKind) -> Self {
        Self {
            span,
            kind,
        }
    }
}