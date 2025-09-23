#pragma once

#include "def.h"

constexpr i32 PHASE[] = { 0, 1, 1, 2, 4, 0 };

struct Trace
{
    i32 material[2][5] {};
    i32 pst_rank[2][48] {};
    i32 pst_file[2][48] {};
    i32 mobility[2][5] {};
    i32 passer[2][6] {};
    i32 phalanx[2][6] {};
    i32 threat[2][4] {};
    i32 king_attack[2][5] {};
    i32 bishop_pair[2] {};
    i32 king_open[2] {};
    i32 king_semiopen[2] {};
    i32 rook_open[2] {};
    i32 rook_semiopen[2] {};
    i32 pawn_protected[2] {};
    i32 pawn_doubled[2] {};
    i32 pawn_shield[2] {};

    f64 score = 0.0;
    f64 scale = 1.0;
    i32 phase = 0;
};

i32 MATERIAL[5] = {
    S(89, 147), S(350, 521), S(361, 521), S(479, 956), S(1046, 1782)
};

i32 PST_RANK[48] = {
    0, S(-3, 0), S(-3, -1), S(-1, -1), S(1, 0), S(5, 3), 0, 0,
    S(-2, -5), S(0, -3), S(1, -1), S(3, 3), S(4, 4), S(5, 1), S(2, 0), S(-15, 1),
    S(0, -2), S(2, -1), S(2, 0), S(2, 0), S(2, 0), S(2, 0), S(-1, 0), S(-10, 2),
    S(0, -3), S(-1, -3), S(-2, -2), S(-2, 0), S(0, 2), S(2, 2), S(1, 3), S(2, 1),
    S(2, -11), S(3, -8), S(2, -3), S(0, 2), S(0, 5), S(-1, 5), S(-4, 7), S(-2, 4),
    S(-1, -6), S(1, -2), S(-1, 0), S(-4, 3), S(-1, 5), S(5, 4), S(5, 2), S(5, -6)
};

i32 PST_FILE[48] = {
    S(-1, 1), S(-2, 1), S(-1, 0), S(0, -1), S(1, 0), S(2, 0), S(2, 0), S(-1, 0),
    S(-4, -3), S(-1, -1), S(0, 1), S(2, 3), S(2, 3), S(2, 0), S(1, -1), S(-1, -3),
    S(-2, -1), 0, S(1, 0), S(0, 1), S(1, 1), S(0, 1), S(2, 0), S(-1, -1),
    S(-2, 0), S(-1, 1), S(0, 1), S(1, 0), S(2, -1), S(1, 0), S(1, 0), S(-1, -1),
    S(-2, -3), S(-1, -1), S(-1, 0), S(0, 1), S(0, 2), S(1, 2), S(2, 0), S(1, -1),
    S(-2, -5), S(2, -1), S(-1, 1), S(-4, 2), S(-4, 2), S(-2, 2), S(2, -1), S(0, -5),
};

i32 MOBILITY[5] = {
    S(8, 5), S(7, 7), S(3, 5), S(3, 2), S(-5, -1)
};

i32 PASSER[6] = {
    S(0, 2), S(-1, 4), S(-2, 6), S(2, 10), S(3, 20), S(5, 30)
};

i32 PHALANX[6] = {
    S(1, 0), S(2, 1), S(3, 2), S(5, 10), S(10, 15), S(15, 20)
};

i32 THREAT[4] = {
    S(50, 25), S(50, 50), S(80, 25), S(75, 0)
};

i32 KING_ATTACK[4] = {
    S(20, 0), S(25, 0), S(25, 0), S(25, 0)
};

i32 BISHOP_PAIR = S(25, 55);
i32 KING_OPEN = S(-75, 5);
i32 KING_SEMIOPEN = S(-30, 15);
i32 ROOK_OPEN = S(25, 5);
i32 ROOK_SEMIOPEN = S(10, 15);
i32 PAWN_PROTECTED = S(12, 16);
i32 PAWN_DOUBLED = S(12, 40);
i32 PAWN_SHIELD = S(30, -10);

i32 TEMPO = 20;

inline Trace get_trace(Board& board)
{
    auto trace = Trace();

    i32 score = 0;
    i32 phase = 0;

    for (i8 color = color::WHITE; color < 2; ++color) {
        u64 occupied = board.colors[color::WHITE] | board.colors[color::BLACK];
        u64 pawns_us = board.pieces[piece::type::PAWN] & board.colors[color];
        u64 pawns_them = board.pieces[piece::type::PAWN] & board.colors[!color];
        u64 pawns_threats = attack::get_pawn_span<color::BLACK>(pawns_them);
        u64 pawns_attacks = attack::get_pawn_span<color::WHITE>(pawns_us);
        u64 pawns_phalanx = bitboard::get_shift<direction::WEST>(pawns_us) & pawns_us;

        // Bishop pair
        if (bitboard::get_count(board.pieces[piece::type::BISHOP] & board.colors[color]) > 1) {
            score += BISHOP_PAIR;
            trace.bishop_pair[color] += 1;
        }

        // Pawn protected
        i32 pawn_protected = bitboard::get_count(pawns_us & pawns_attacks);

        score += pawn_protected * PAWN_PROTECTED;
        trace.pawn_protected[color] += pawn_protected;

        // Pawn f64d
        i32 pawn_doubled = bitboard::get_count(pawns_us & (pawns_us << 8 | pawns_us << 16));

        score += pawn_doubled * PAWN_DOUBLED;
        trace.pawn_doubled[color] += pawn_doubled;

        for (i8 type = piece::type::PAWN; type < 6; ++type) {
            u64 mask = board.pieces[type] & board.colors[color];

            while (mask) {
                i32 square = bitboard::pop_lsb(mask);

                // Phase
                phase += PHASE[type];

                // Material
                if (type < piece::type::KING) {
                    score += MATERIAL[type];
                    trace.material[color][type] += 1;
                }

                // PST rank
                score += PST_RANK[type * 8 + square / 8];
                trace.pst_rank[color][type * 8 + square / 8] += 1;

                // PST file
                score += PST_FILE[type * 8 + square % 8];
                trace.pst_file[color][type * 8 + square % 8] += 1;

                if (type == piece::type::PAWN) {
                    // Pawn phalanx
                    if (pawns_phalanx & 1ULL << square) {
                        score += PHALANX[square / 8 - 1];
                        trace.phalanx[color][square / 8 - 1] += 1;
                    }

                    // Passed pawns
                    if (!(0x101010101010101ULL << square & (pawns_them | pawns_threats))) {
                        score += PASSER[square / 8 - 1];
                        trace.passer[color][square / 8 - 1] += 1;
                    }
                }
                else {
                    // Mobility
                    u64 attack = 0ULL;

                    switch (type)
                    {
                    case piece::type::KNIGHT:
                        attack = attack::get_knight(square);
                        break;
                    case piece::type::BISHOP:
                        attack = attack::get_bishop(square, occupied);
                        break;
                    case piece::type::ROOK:
                        attack = attack::get_rook(square, occupied);
                        break;
                    case piece::type::QUEEN:
                        attack = attack::get_bishop(square, occupied) | attack::get_rook(square, occupied);
                        break;
                    case piece::type::KING:
                        attack = attack::get_king(square);
                        break;
                    default:
                        break;
                    }

                    i32 mobility = bitboard::get_count(attack & ~board.colors[color] & ~pawns_threats);

                    score += mobility * MOBILITY[type - 1];
                    trace.mobility[color][type - 1] += mobility;

                    // Open file
                    if (!(0x101010101010101ULL << square % 8 & board.pieces[piece::type::PAWN])) {
                        // King
                        if (type == piece::type::KING) {
                            score += KING_OPEN;
                            trace.king_open[color] += 1;
                        }

                        // Rook
                        if (type == piece::type::ROOK) {
                            score += ROOK_OPEN;
                            trace.rook_open[color] += 1;
                        }
                    }

                    // Semi open file
                    if (!(0x101010101010101 << square % 8 & pawns_us)) {
                        // King
                        if (type == piece::type::KING) {
                            score += KING_SEMIOPEN;
                            trace.king_semiopen[color] += 1;
                        }

                        // Rook
                        if (type == piece::type::ROOK) {
                            score += ROOK_SEMIOPEN;
                            trace.rook_semiopen[color] += 1;
                        }
                    }

                    // Pawn shield
                    if (type == piece::type::KING && square < 8) {
                        i32 pawn_shield = bitboard::get_count(pawns_us & 0x70700ULL << 5 * (square % 8 > 2));

                        score += pawn_shield * PAWN_SHIELD;
                        trace.pawn_shield[color] += pawn_shield;
                    }

                    // Pawn threats
                    if (1ULL << square & pawns_threats) {
                        score += THREAT[type - 1];
                        trace.threat[color][type - 1] += 1;
                    }

                    // King attacker
                    if (type < piece::type::KING) {
                        i32 king_attack = bitboard::get_count(attack & attack::get_king(bitboard::get_lsb(board.pieces[piece::type::KING] & board.colors[!color])));

                        score += king_attack * KING_ATTACK[type - 1];
                        trace.king_attack[color][type - 1] += king_attack;
                    }
                }
            }
        }

        // Flip board
        board.colors[color::WHITE] = bitboard::get_flip(board.colors[color::WHITE]);
        board.colors[color::BLACK] = bitboard::get_flip(board.colors[color::BLACK]);

        for (auto& piece : board.pieces)
            piece = bitboard::get_flip(piece);

        score = -score;
    }

    // Scaling
    i32 x = 8 - bitboard::get_count(board.pieces[piece::type::PAWN] & board.colors[score < 0]);
    i32 scale = 128 - x * x;
    i32 mg = get_mg(score);
    i32 eg = get_eg(score);
    
    score = (mg * phase + eg * scale / 128 * (24 - phase)) / 24;

    trace.scale = f64(scale) / 128.0;
    trace.score = score;
    trace.phase = phase;

    return trace;
};

void add_weight(std::vector<Pair>& weights, i32 value)
{
    i32 mg = get_mg(value);
    i32 eg = get_eg(value);

    Pair pair = { f64(mg), f64(eg) };

    weights.push_back(pair);
};

void add_coef(std::vector<i32>& coefs, i32 trace[2])
{
    coefs.push_back(trace[color::WHITE] - trace[color::BLACK]);
};

std::string get_str_score(Pair value)
{
    i32 mg = std::round(value[MG]);
    i32 eg = std::round(value[EG]);

    std::string str;

    str += "S(";
    str += std::to_string(mg);
    str += ", ";
    str += std::to_string(eg);
    str += ")";

    return str;
};

std::string get_str_weight(std::vector<Pair>& weights, i32& index, std::string name)
{
    std::string str = "i32 " + name + " = " + get_str_score(weights[index]) + ";\n";

    index += 1;

    return str;
};

std::string get_str_weights(std::vector<Pair>& weights, i32& index, std::string name, i32 count)
{
    std::string str = "i32 " + name + "[] = { ";

    for (i32 i = 0; i < count; ++i) {
        str += get_str_score(weights[index]);

        index += 1;

        if (i < count - 1) {
            str += ", ";
        }
    }

    str += " };\n";

    return str;
};

#define add_weights(weights, values, count) for (i32 i = 0; i < count; ++i) add_weight(weights, values[i]);

#define add_coefs(coefs, trace, count) for (i32 i = 0; i < count; ++i) coefs.push_back(trace[color::WHITE][i] - trace[color::BLACK][i]);

std::vector<Pair> get_init_weights()
{
    std::vector<Pair> result;

    add_weights(result, MATERIAL, 5);
    add_weights(result, PST_RANK, 48);
    add_weights(result, PST_FILE, 48);
    add_weights(result, MOBILITY, 5);
    add_weights(result, PASSER, 6);
    add_weights(result, PHALANX, 6);
    add_weights(result, THREAT, 4);
    add_weights(result, KING_ATTACK, 4);

    add_weight(result, BISHOP_PAIR);
    add_weight(result, KING_OPEN);
    add_weight(result, KING_SEMIOPEN);
    add_weight(result, ROOK_OPEN);
    add_weight(result, ROOK_SEMIOPEN);
    add_weight(result, PAWN_PROTECTED);
    add_weight(result, PAWN_DOUBLED);
    add_weight(result, PAWN_SHIELD);

    return result;
};

std::vector<i32> get_coefs(Trace trace)
{
    std::vector<i32> result;

    add_coefs(result, trace.material, 5);
    add_coefs(result, trace.pst_rank, 48);
    add_coefs(result, trace.pst_file, 48);
    add_coefs(result, trace.mobility, 5);
    add_coefs(result, trace.passer, 6);
    add_coefs(result, trace.phalanx, 6);
    add_coefs(result, trace.threat, 4);
    add_coefs(result, trace.king_attack, 4);

    add_coef(result, trace.bishop_pair);
    add_coef(result, trace.king_open);
    add_coef(result, trace.king_semiopen);
    add_coef(result, trace.rook_open);
    add_coef(result, trace.rook_semiopen);
    add_coef(result, trace.pawn_protected);
    add_coef(result, trace.pawn_doubled);
    add_coef(result, trace.pawn_shield);

    return result;
};

std::string get_str_print_weights(std::vector<Pair> weights)
{
    std::string str;
    i32 index = 0;

    str += get_str_weights(weights, index, "MATERIAL", 5);
    str += get_str_weights(weights, index, "PST_RANK", 48);
    str += get_str_weights(weights, index, "PST_FILE", 48);
    str += get_str_weights(weights, index, "MOBILITY", 5);
    str += get_str_weights(weights, index, "PASSER", 6);
    str += get_str_weights(weights, index, "PHALANX", 6);
    str += get_str_weights(weights, index, "THREAT", 4);
    str += get_str_weights(weights, index, "KING_ATTACK", 4);

    str += get_str_weight(weights, index, "BISHOP_PAIR");
    str += get_str_weight(weights, index, "KING_OPEN");
    str += get_str_weight(weights, index, "KING_SEMIOPEN");
    str += get_str_weight(weights, index, "ROOK_OPEN");
    str += get_str_weight(weights, index, "ROOK_SEMIOPEN");
    str += get_str_weight(weights, index, "PAWN_PROTECTED");
    str += get_str_weight(weights, index, "PAWN_DOUBLED");
    str += get_str_weight(weights, index, "PAWN_SHIELD");

    return str;
};

void print_weights(std::vector<Pair> weights)
{
    std::cout << get_str_print_weights(weights);
};