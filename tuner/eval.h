#pragma once

#include "def.h"

constexpr i32 PHASE[] = { 0, 1, 1, 2, 4, 0 };

struct Trace
{
    i32 pst_rank[2][48] {};
    i32 pst_file[2][48] {};
    i32 mobility[2][5] {};
    i32 passer[2][6] {};
    i32 phalanx[2][6] {};
    i32 threat[2][4] {};
    i32 push_threat[2][4] {};
    i32 king_attack[2][5] {};
    i32 king_passer_us[2][8] {};
    i32 king_passer_them[2][8] {};
    i32 bishop_pair[2] {};
    i32 king_open[2] {};
    i32 king_semiopen[2] {};
    i32 king_pawn_threat[2] {};
    i32 rook_open[2] {};
    i32 rook_semiopen[2] {};
    i32 pawn_protected[2] {};
    i32 pawn_doubled[2] {};
    i32 pawn_shield[2] {};
    i32 passer_blocked[2] {};

    f64 score = 0.0;
    f64 scale = 1.0;
    i32 phase = 0;
    i32 delta = 0;
};

i32 MATERIAL[5] = {
    S(95, 206), S(360, 586), S(375, 595), S(506, 1062), S(1271, 1795)
};

i32 PST_RANK[] = {
    S(0, 0), S(-22, -25), S(-35, -36), S(-27, -32), S(-11, -23), S(45, 13), S(52, 120), S(0, 0),
    S(-30, -28), S(-10, -11), S(5, 7), S(26, 32), S(46, 36), S(60, 6), S(35, -14), S(-131, -28),
    S(-6, -15), S(11, -11), S(17, 4), S(16, 6), S(27, 6), S(21, 2), S(-11, -3), S(-75, 11),
    S(3, -25), S(-8, -27), S(-6, -22), S(-15, 2), S(9, 11), S(17, 10), S(-2, 31), S(1, 20),
    S(9, -52), S(16, -48), S(5, -8), S(1, 15), S(1, 34), S(9, 30), S(-18, 34), S(-24, -4),
    S(-20, -32), S(42, -12), S(-37, 6), S(-102, 21), S(-81, 26), S(21, 27), S(35, 23), S(21, -49)
};
i32 PST_FILE[] = { 
    S(-19, 14), S(-9, 14), S(-5, -4), S(5, -16), S(15, -9), S(13, -1), S(14, 6), S(-14, -5),
    S(-41, -20), S(-14, -3), S(-3, 15), S(15, 19), S(17, 18), S(19, 4), S(8, -5), S(-2, -29),
    S(-18, -6), S(2, 0), S(4, -1), S(-3, 11), S(4, 9), S(-1, 7), S(13, -1), S(-1, -18),
    S(-14, 5), S(-15, 11), S(-6, 13), S(4, 1), S(16, -10), S(4, -1), S(17, -8), S(-6, -11),
    S(-15, -19), S(-12, -5), S(-12, 12), S(-4, 15), S(-2, 21), S(6, 5), S(20, -11), S(18, -18),
    S(5, -56), S(38, -18), S(-1, 13), S(-58, 34), S(-24, 22), S(-44, 22), S(29, -8), S(16, -53)
};
i32 MOBILITY[] = { S(9, 5), S(8, 7), S(3, 6), S(2, 10), S(-9, -2) };
i32 PASSER[] = { S(15, -27), S(10, 8), S(2, 72), S(12, 138), S(-36, 235), S(20, 228) };
i32 PHALANX[] = { S(9, 4), S(17, 21), S(32, 37), S(59, 102), S(159, 293), S(154, 294) };
i32 THREAT[] = { S(76, 27), S(81, 56), S(104, -6), S(89, -58) };
i32 PUSH_THREAT[] = { S(29, 9), S(29, -7), S(20, 19), S(25, -16) };
i32 KING_ATTACK[] = { S(8, 20), S(16, 31), S(45, -27), S(18, 19) };
i32 KING_PASSER_US[] = { S(-74, 68), S(-49, 94), S(-14, 37), S(-23, 13), S(-3, -18), S(7, -35), S(40, -49), S(8, -37) };
i32 KING_PASSER_THEM[] = { S(-97, -4), S(40, -80), S(1, -41), S(-16, -6), S(-21, 24), S(-28, 44), S(-52, 68), S(-64, 60) };
i32 BISHOP_PAIR = S(30, 96);
i32 KING_OPEN = S(-69, -7);
i32 KING_SEMIOPEN = S(-33, 13);
i32 KING_PAWN_THREAT = S(14, 53);
i32 ROOK_OPEN = S(25, -5);
i32 ROOK_SEMIOPEN = S(15, 19);
i32 PAWN_PROTECTED = S(23, 28);
i32 PAWN_DOUBLED = S(10, 39);
i32 PAWN_SHIELD = S(29, -13);
// i32 PASSER_BLOCKED = S(-10, 72);

i32 TEMPO = 20;

inline Trace get_trace(Board& board, f64 wdl)
{
    auto trace = Trace();

    i32 score = 0;
    i32 phase = 0;
    i32 phases[2] = { 0 };

    i32 material = 0;

    for (i8 color = color::WHITE; color < 2; ++color) {
        u64 occupied = board.colors[color::WHITE] | board.colors[color::BLACK];
        u64 pawns_us = board.pieces[piece::type::PAWN] & board.colors[color];
        u64 pawns_them = board.pieces[piece::type::PAWN] & board.colors[!color];
        u64 pawns_threats = attack::get_pawn_span<color::BLACK>(pawns_them);
        u64 pawns_attacks = attack::get_pawn_span<color::WHITE>(pawns_us);
        u64 pawns_phalanx = bitboard::get_shift<direction::WEST>(pawns_us) & pawns_us;
        u64 pawns_push_threats = attack::get_pawn_span<color::BLACK>(bitboard::get_shift<direction::SOUTH>(pawns_them) & ~occupied);

        i8 king_square_us = bitboard::get_lsb(board.pieces[piece::type::KING] & board.colors[color]);
        i8 king_square_them = bitboard::get_lsb(board.pieces[piece::type::KING] & board.colors[!color]);

        // Bishop pair
        if (bitboard::get_count(board.pieces[piece::type::BISHOP] & board.colors[color]) > 1) {
            score += BISHOP_PAIR;
            trace.bishop_pair[color] += 1;
        }

        // Pawn protected
        i32 pawn_protected = bitboard::get_count(pawns_us & pawns_attacks);

        score += pawn_protected * PAWN_PROTECTED;
        trace.pawn_protected[color] += pawn_protected;

        // Pawn doubled
        i32 pawn_doubled = bitboard::get_count(pawns_us & (pawns_us << 8 | pawns_us << 16));

        score -= pawn_doubled * PAWN_DOUBLED;
        trace.pawn_doubled[color] -= pawn_doubled;

        // King threatening pawns
        i32 pawn_threaten = bitboard::get_count(attack::get_king(king_square_us) & pawns_them);

        score += pawn_threaten * KING_PAWN_THREAT;
        trace.king_pawn_threat[color] += pawn_threaten;

        for (i8 type = piece::type::PAWN; type < 6; ++type) {
            u64 mask = board.pieces[type] & board.colors[color];

            while (mask) {
                i32 square = bitboard::pop_lsb(mask);

                // Phase
                phase += PHASE[type];
                phases[color] += PHASE[type];

                // Material
                if (type < piece::type::KING) {
                    score += MATERIAL[type];
                    material += MATERIAL[type];
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

                        // Blocked passed pawn
                        // if (((1ULL << square) << 8) & board.colors[!color]) {
                        //     score -= PASSER_BLOCKED;
                        //     trace.passer_blocked[color] -= 1;
                        // }

                        // King distance
                        i32 king_passer_us = std::max(std::abs(square / 8 - king_square_us / 8 + 1), std::abs(square % 8 - king_square_us % 8));
                        i32 king_passer_them = std::max(std::abs(square / 8 - king_square_them / 8 + 1), std::abs(square % 8 - king_square_them % 8));

                        score += KING_PASSER_US[king_passer_us];
                        trace.king_passer_us[color][king_passer_us] += 1;

                        score += KING_PASSER_THEM[king_passer_them];
                        trace.king_passer_them[color][king_passer_them] += 1;
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
                    if (!(0x101010101010101ULL << square % 8 & pawns_us)) {
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
                        score -= THREAT[type - 1];
                        trace.threat[color][type - 1] -= 1;
                    }

                    // Pawn push threats
                    if (1ULL << square & pawns_push_threats) {
                        score -= PUSH_THREAT[type - 1];
                        trace.push_threat[color][type - 1] -= 1;
                    }

                    // King attacker
                    if (type < piece::type::KING && board.pieces[piece::type::QUEEN]) {
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
        material = -material;
    }

    // Scaling
    i32 strong = wdl < 0.5;
    i32 x = bitboard::get_count(board.pieces[piece::type::PAWN] & board.colors[strong]);
    i32 scale = x == 0 && (phases[strong] - phases[!strong] < 2 || phases[strong] < 3 && bitboard::get_count(board.pieces[piece::type::KNIGHT]) < 3) ? 1 : 8 + x;
    i32 mg = get_mg(score);
    i32 eg = get_eg(score);
    
    score = (mg * phase + eg * scale / 16 * (24 - phase)) / 24;

    trace.scale = f64(scale) / 16.0;
    trace.score = score;
    trace.phase = phase;
    trace.delta = material;

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
    std::string str = "i32 " + name + "[] = {";

    if (count > 8) {
        str += "\n";
    }
    else {
        str += " ";
    }

    for (i32 i = 0; i < count; ++i) {
        str += get_str_score(weights[index]);

        index += 1;

        if (i < count - 1) {
            str += ", ";

            if ((i + 1) % 8 == 0) {
                str += "\n";
            }
        }
    }

    if (count > 8) {
        str += "\n";
    }
    else {
        str += " ";
    }

    str += "};\n";

    return str;
};

#define add_weights(weights, values, count) for (i32 i = 0; i < count; ++i) add_weight(weights, values[i]);

#define add_coefs(coefs, trace, count) for (i32 i = 0; i < count; ++i) coefs.push_back(trace[color::WHITE][i] - trace[color::BLACK][i]);

std::vector<Pair> get_init_weights()
{
    std::vector<Pair> result;

    add_weights(result, PST_RANK, 48);
    add_weights(result, PST_FILE, 48);
    add_weights(result, MOBILITY, 5);
    add_weights(result, PASSER, 6);
    add_weights(result, PHALANX, 6);
    add_weights(result, THREAT, 4);
    add_weights(result, PUSH_THREAT, 4);
    add_weights(result, KING_ATTACK, 4);
    add_weights(result, KING_PASSER_US, 8);
    add_weights(result, KING_PASSER_THEM, 8);

    add_weight(result, BISHOP_PAIR);
    add_weight(result, KING_OPEN);
    add_weight(result, KING_SEMIOPEN);
    add_weight(result, KING_PAWN_THREAT);
    add_weight(result, ROOK_OPEN);
    add_weight(result, ROOK_SEMIOPEN);
    add_weight(result, PAWN_PROTECTED);
    add_weight(result, PAWN_DOUBLED);
    add_weight(result, PAWN_SHIELD);
    // add_weight(result, PASSER_BLOCKED);

    return result;
};

std::vector<i32> get_coefs(Trace trace)
{
    std::vector<i32> result;

    add_coefs(result, trace.pst_rank, 48);
    add_coefs(result, trace.pst_file, 48);
    add_coefs(result, trace.mobility, 5);
    add_coefs(result, trace.passer, 6);
    add_coefs(result, trace.phalanx, 6);
    add_coefs(result, trace.threat, 4);
    add_coefs(result, trace.push_threat, 4);
    add_coefs(result, trace.king_attack, 4);
    add_coefs(result, trace.king_passer_us, 8);
    add_coefs(result, trace.king_passer_them, 8);

    add_coef(result, trace.bishop_pair);
    add_coef(result, trace.king_open);
    add_coef(result, trace.king_semiopen);
    add_coef(result, trace.king_pawn_threat);
    add_coef(result, trace.rook_open);
    add_coef(result, trace.rook_semiopen);
    add_coef(result, trace.pawn_protected);
    add_coef(result, trace.pawn_doubled);
    add_coef(result, trace.pawn_shield);
    // add_coef(result, trace.passer_blocked);

    return result;
};

std::string get_str_print_weights(std::vector<Pair> weights)
{
    std::string str;
    i32 index = 0;

    std::vector<Pair> pst_rank;
    std::vector<Pair> pst_file;
    std::vector<Pair> material;

    add_weights(material, MATERIAL, 5);

    for (usize i = 0; i < 48; ++i) {
        pst_rank.push_back(weights[i]);
        pst_file.push_back(weights[i + 48]);
    }

    for (usize type = 0; type < 5; ++type) {
        Pair pst_rank_avg = { 0.0, 0.0 };
        Pair pst_file_avg = { 0.0, 0.0 };

        for (usize i = 0; i < 8; ++i) {
            pst_rank_avg[MG] += pst_rank[type * 8 + i][MG] / 8;
            pst_rank_avg[EG] += pst_rank[type * 8 + i][EG] / 8;

            pst_file_avg[MG] += pst_file[type * 8 + i][MG] / 8;
            pst_file_avg[EG] += pst_file[type * 8 + i][EG] / 8;
        }

        material[type][MG] += pst_rank_avg[MG] + pst_file_avg[MG];
        material[type][EG] += pst_rank_avg[EG] + pst_file_avg[EG];

        for (usize i = 0; i < 8; ++i) {
            weights[type * 8 + i][MG] -= pst_rank_avg[MG];
            weights[type * 8 + i][EG] -= pst_rank_avg[EG];

            weights[type * 8 + i + 48][MG] -= pst_file_avg[MG];
            weights[type * 8 + i + 48][EG] -= pst_file_avg[EG];
        }
    }

    i32 index_material = 0;

    str += get_str_weights(material, index_material, "MATERIAL", 5);
    str += get_str_weights(weights, index, "PST_RANK", 48);
    str += get_str_weights(weights, index, "PST_FILE", 48);
    str += get_str_weights(weights, index, "MOBILITY", 5);
    str += get_str_weights(weights, index, "PASSER", 6);
    str += get_str_weights(weights, index, "PHALANX", 6);
    str += get_str_weights(weights, index, "THREAT", 4);
    str += get_str_weights(weights, index, "PUSH_THREAT", 4);
    str += get_str_weights(weights, index, "KING_ATTACK", 4);
    str += get_str_weights(weights, index, "KING_PASSER_US", 8);
    str += get_str_weights(weights, index, "KING_PASSER_THEM", 8);

    str += get_str_weight(weights, index, "BISHOP_PAIR");
    str += get_str_weight(weights, index, "KING_OPEN");
    str += get_str_weight(weights, index, "KING_SEMIOPEN");
    str += get_str_weight(weights, index, "KING_PAWN_THREAT");
    str += get_str_weight(weights, index, "ROOK_OPEN");
    str += get_str_weight(weights, index, "ROOK_SEMIOPEN");
    str += get_str_weight(weights, index, "PAWN_PROTECTED");
    str += get_str_weight(weights, index, "PAWN_DOUBLED");
    str += get_str_weight(weights, index, "PAWN_SHIELD");
    // str += get_str_weight(weights, index, "PASSER_BLOCKED");

    str += "i32 TEMPO = " + std::to_string(std::round(TEMPO)) + ";\n";

    return str;
};

void print_weights(std::vector<Pair> weights)
{
    std::cout << get_str_print_weights(weights);
};