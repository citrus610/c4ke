#include "board.h"

Board::Board()
{
    for (auto& color : this->colors) {
        color = 0ULL;
    }

    for (auto& piece : this->pieces) {
        piece = 0ULL;
    }

    for (auto& square : board) {
        square = piece::NONE;
    }

    this->stm = color::WHITE;
};

void Board::set(const std::string& fen)
{
    // Zero init
    *this = Board();

    // Reads fen
    std::stringstream ss(fen);
    std::string word;
    std::vector<std::string> data;

    while (std::getline(ss, word, ' '))
    {
        data.push_back(word);
    }

    assert(data.size() >= 4);
    assert(!data[0].empty());

    auto str_board = data.size() > 0 ? data[0] : "";
    auto str_color = data.size() > 1 ? data[1] : "w";
    auto str_castling = data.size() > 2 ? data[2] : "-";
    auto str_enpassant = data.size() > 3 ? data[3] : "-";
    auto str_halfmove = data.size() > 4 ? data[4] : "0";
    auto str_fullmove = data.size() > 5 ? data[5] : "1";

    // Sets board
    auto square = 56;

    for (char c : str_board) {
        if (std::isdigit(c)) {
            square += c - '0';
        }
        else if (c == '/') {
            square -= 16;
        }
        else {
            i8 piece = piece::create(c);
            i8 piece_type = piece::get_type(piece);
            i8 piece_color = piece::get_color(piece);

            this->board[square] = piece;
            this->pieces[piece_type] |= bitboard::create(square);
            this->colors[piece_color] |= bitboard::create(square);

            square += 1;
        }
    }

    // Sets color
    this->stm = str_color == "w" ? color::WHITE : color::BLACK;
};

void Board::print()
{
    for (i32 rank = 7; rank >= 0; --rank) {
        char line[] = ". . . . . . . .";

        for (i32 file = 0; file < 8; ++file) {
            i8 square = square::create(file, rank);

            if (this->board[square] == piece::NONE) {
                continue;
            }

            line[2 * file] = piece::get_char(this->board[square]);
        }

        printf("%s\n", line);
    }

    printf("\n");
};