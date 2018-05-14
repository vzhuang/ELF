/**
 * Copyright (c) 2018-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <random>
#include <string>

#include "elf/legacy/python_options_utils_cpp.h"
#include "elf/logging/IndexedLoggerFactory.h"

#include "game_base.h"
#include "game_ctrl.h"
#include "game_feature.h"
#include "game_stats.h"
#include "mcts/mcts.h"
#include "sgf/sgf.h"

// Game interface for Go.
class GoGameSelfPlay : public GoGameBase {
 public:
  GoGameSelfPlay(
      int game_idx,
      elf::GameClient* client,
      const ContextOptions& context_options,
      const GameOptions& options,
      EvalCtrl* eval_ctrl);

  void act() override;

  std::string showBoard() const {
    return _state_ext.state().showBoard();
  }
  std::string getNextPlayer() const {
    return player2str(_state_ext.state().nextPlayer());
  }
  std::string getLastMove() const {
    return coord2str2(_state_ext.lastMove());
  }
  float getScore() {
    return _state_ext.state().evaluate(_options.komi);
  }

  float getLastScore() const {
    return _state_ext.getLastGameFinalValue();
  }

 private:
  void check_new_request();
  void setAsync();

  MCTSGoAI* init_ai(
      const std::string& actor_name,
      const elf::ai::tree_search::TSOptions& mcts_opt,
      float second_puct,
      int second_mcts_rollout_per_batch,
      int second_mcts_rollout_per_thread,
      int64_t model_ver);
  Coord mcts_make_diverse_move(MCTSGoAI* curr_ai, Coord c);
  Coord mcts_update_info(MCTSGoAI* mcts_go_ai, Coord c);

  void restart();
  void finish_game(FinishReason reason);

  static elf::logging::IndexedLoggerFactory* getLoggerFactory();

 private:
  EvalCtrl* eval_ctrl_ = nullptr;

  GoStateExt _state_ext;

  Sgf _preload_sgf;
  Sgf::iterator _sgf_iter;

  int _online_counter = 0;

  std::unique_ptr<MCTSGoAI> _ai;
  // Opponent ai (used for selfplay evaluation)
  std::unique_ptr<MCTSGoAI> _ai2;
  std::unique_ptr<AI> _human_player;

  std::shared_ptr<spdlog::logger> logger_;
};
