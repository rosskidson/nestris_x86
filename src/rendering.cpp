#include "rendering.hpp"

#include <memory>
#include <set>

#include "assets.hpp"
#include "game_logic.hpp"
#include "game_processor.hpp"
#include "game_states.hpp"
#include "key_defines.hpp"
#include "logging.hpp"
#include "olcPixelGameEngine.h"
#include "option.hpp"

namespace tetris_clone {

struct Coords {
  int x;
  int y;
};

struct Rect {
  int width;
  int height;
};

Renderer::Renderer(olc::PixelGameEngine &render_engine, const std::string &sprites_path)
    : render_engine_ref_(render_engine), frame_counter_(0) {
  if (not loadBlockSprites(sprites_path, block_sprites_)) {
    throw std::runtime_error("Failed loading block sprites.");
  }
  if (not loadSprites(sprites_path, sprite_map_)) {
    throw std::runtime_error("Failed loading sprites.");
  }
}

void Renderer::drawNumber(const int x, const int y, const int num, const int pad,
                          const olc::Pixel color) {
  constexpr int TEXT_WIDTH_PX = 8;
  render_engine_ref_.FillRect(x, y, TEXT_WIDTH_PX * pad, TEXT_WIDTH_PX, olc::BLACK);
  std::stringstream ss;
  ss << std::setw(pad) << std::setfill('0') << num;
  render_engine_ref_.DrawString(x, y, ss.str(), color);
}

void Renderer::renderText(const GameState<> &state) {
  constexpr Coords lines_pos{152, 16};
  constexpr Coords high_score_pos{192, 32};
  constexpr Coords score_pos{192, 56};
  constexpr Coords level_pos{208, 160};
  drawNumber(lines_pos.x, lines_pos.y, state.lines, 3);
  drawNumber(high_score_pos.x, high_score_pos.y, state.high_score, 6);
  drawNumber(score_pos.x, score_pos.y, state.score, 6);
  drawNumber(level_pos.x, level_pos.y, state.level, 2);

  constexpr Coords tetromino_counter_start{48, 88};
  for (int i = 0; i < 7; ++i) {
    drawNumber(tetromino_counter_start.x, tetromino_counter_start.y + (i * 16),
               state.tetromino_counts[i], 3, olc::RED);
  }
}

void Renderer::renderNextTetromino(const Tetromino &next_tetromino, const int level) {
  auto get_next_tetromino_plotting_coords =
      [](const Tetromino &next_tetromino) -> std::tuple<int, int> {
    constexpr int start_x = 196;
    constexpr int start_y = 104;
    if (next_tetromino == Tetromino::Square) {
      return {start_x + 4, start_y + 8};
    } else if (next_tetromino == Tetromino::Line) {
      return {start_x - 4, start_y - 4};
    } else {
      return {start_x, start_y};
    }
  };

  // Clear existing tetromino.
  render_engine_ref_.FillRect(191, 112, 33, 15, olc::BLACK);

  const auto tetromino = getTetrominoGrid(next_tetromino, 0);
  const auto [x, y] = get_next_tetromino_plotting_coords(next_tetromino);
  renderGrid(x, y, tetromino, level, 8, 8, getColor(next_tetromino));
}

void Renderer::renderEntryDelay(const bool delay_entry, const int x, const int y) {
  const auto delay_sprite = delay_entry ? "are-on" : "are-off";
  render_engine_ref_.DrawSprite(x, y, getSprite(delay_sprite));
}

void Renderer::renderDasBar(const int das_counter, const Das &das_processor, const int x,
                            const int y) {
  const Coords das_box_pos{x, y};
  const Coords das_bar_pos{das_box_pos.x + 31, das_box_pos.y + 7};
  const int das_bar_length_pixels = 32;
  const int das_bar_width_pixels = 8;
  render_engine_ref_.DrawSprite(das_box_pos.x, das_box_pos.y, getSprite("das_meter"));

  auto get_das_color = [&das_processor](const int &das) {
    const int das_min_charge = das_processor.getMinDasChargeCount();
    if (das >= das_min_charge) {
      return olc::GREEN;
    } else if (das >= das_min_charge / 2) {
      return olc::YELLOW;
    } else {
      return olc::RED;
    };
  };

  auto get_das_bar_pixel_length = [&](const int das) {
    return das_processor.dasFullyCharged(das)
               ? das_bar_length_pixels
               : das_counter * (das_bar_length_pixels / das_processor.getFullDasChargeCount());
  };

  const auto color = get_das_color(das_counter);
  const auto das_bar_pixels = get_das_bar_pixel_length(das_counter);
  render_engine_ref_.FillRect(das_bar_pos.x, das_bar_pos.y, das_bar_pixels, das_bar_width_pixels,
                              color);
}

void Renderer::renderControls(const GameState<> &state, const KeyEvents &key_events, const int x,
                              const int y) {
  const Coords controller_box_pos{x, y};
  const Coords arrow_left{controller_box_pos.x + 7, controller_box_pos.y + 13};
  const Coords arrow_right{controller_box_pos.x + 16, controller_box_pos.y + 13};
  const Coords arrow_up{controller_box_pos.x + 11, controller_box_pos.y + 9};
  const Coords arrow_down{controller_box_pos.x + 11, controller_box_pos.y + 17};
  const Coords a_button{controller_box_pos.x + 55, controller_box_pos.y + 15};
  const Coords b_button{controller_box_pos.x + 46, controller_box_pos.y + 15};
  const Coords start_button{controller_box_pos.x + 34, controller_box_pos.y + 17};

  render_engine_ref_.DrawSprite(controller_box_pos.x, controller_box_pos.y,
                                getSprite("controller"));
  auto key_action_to_bool = [](const KeyEvent &key_event) {
    return key_event.held || key_event.pressed;
  };

  if (key_action_to_bool(key_events.at(KeyAction::Left))) {
    render_engine_ref_.FillRect(arrow_left.x, arrow_left.y, 4, 4, olc::GREEN);
  }
  if (key_action_to_bool(key_events.at(KeyAction::Right))) {
    render_engine_ref_.FillRect(arrow_right.x, arrow_right.y, 4, 4, olc::GREEN);
  }
  if (key_action_to_bool(key_events.at(KeyAction::Up))) {
    render_engine_ref_.FillRect(arrow_up.x, arrow_up.y, 5, 4, olc::GREEN);
  }
  if (key_action_to_bool(key_events.at(KeyAction::Down))) {
    render_engine_ref_.FillRect(arrow_down.x, arrow_down.y, 5, 4, olc::GREEN);
  }
  if (key_action_to_bool(key_events.at(KeyAction::RotateClockwise))) {
    render_engine_ref_.FillRect(a_button.x + 1, a_button.y, 4, 6, olc::GREEN);
    render_engine_ref_.FillRect(a_button.x, a_button.y + 1, 6, 4, olc::GREEN);
  }
  if (key_action_to_bool(key_events.at(KeyAction::RotateAntiClockwise))) {
    render_engine_ref_.FillRect(b_button.x + 1, b_button.y, 4, 6, olc::GREEN);
    render_engine_ref_.FillRect(b_button.x, b_button.y + 1, 6, 4, olc::GREEN);
  }
  if (key_action_to_bool(key_events.at(KeyAction::Start))) {
    render_engine_ref_.FillRect(start_button.x + 1, start_button.y, 4, 3, olc::GREEN);
    render_engine_ref_.FillRect(start_button.x, start_button.y + 1, 6, 1, olc::GREEN);
  }
}

void Renderer::renderBackground(const std::string background_sprite) {
  if (background_rendered_ != background_sprite) {
    render_engine_ref_.DrawSprite(0, 0, getSprite(background_sprite));
    background_rendered_ = background_sprite;
  }
}

// This is a little hacky as it undermines the renderBackground function.
// TODO:: Send line clear animation info to the renderGameState fn.
void Renderer::doTetrisFlash(const int &line_clear_frame_number) {
  const auto &frame = line_clear_frame_number;
  if ((frame - 1) % 4 == 0) {
    render_engine_ref_.DrawSprite(0, 0, getSprite("background_flash"));
  } else {
    render_engine_ref_.DrawSprite(0, 0, getSprite("background-black"));
  }
}

void Renderer::renderPaused() {
  constexpr Coords paused_pos{110, 116};
  render_engine_ref_.DrawString(paused_pos.x, paused_pos.y, "PAUSED", olc::WHITE);
}

void Renderer::renderGameState(const GameState<> &state, const bool render_controls,
                               const bool render_das_bar, const bool render_entry_delay,
                               const KeyEvents &key_events, const Das &das_processor) {
  constexpr Coords grid_top_left{96, 40};
  constexpr Rect grid_size{80, 160};
  constexpr Coords counter_sprite_pos{13, 61};
  constexpr Coords das_box_pos{184, 175};
  constexpr Coords controller_box_pos{184, 196};
  constexpr Coords entry_delay_pos{193, 129};

  auto get_grid_for_render = [](const GameState<> &state) {
    if (entryDelay(state)) {
      return state.grid;
    } else {
      return addTetrominoToGrid(state.grid, state.active_tetromino);
    }
  };
  renderBackground("background-black");
  // Clear the field.
  render_engine_ref_.FillRect(grid_top_left.x, grid_top_left.y, grid_size.width, grid_size.height,
                              olc::BLACK);

  render_engine_ref_.DrawSprite(counter_sprite_pos.x, counter_sprite_pos.y,
                                getSprite("l" + std::to_string(state.level % 10) + "-counts"));
  renderGrid(grid_top_left.x, grid_top_left.y, get_grid_for_render(state), state.level);
  renderNextTetromino(state.next_tetromino, state.level);
  renderText(state);
  if (render_controls) {
    renderControls(state, key_events, controller_box_pos.x, controller_box_pos.y);
  }
  if (render_das_bar) {
    renderDasBar(state.das_counter, das_processor, das_box_pos.x, das_box_pos.y);
  }
  if (render_entry_delay) {
    renderEntryDelay(entryDelay(state), entry_delay_pos.x, entry_delay_pos.y);
  }
  if (state.paused) {
    renderPaused();
  }
}

void Renderer::drawTriangleSelector(const int x, const int y, const int size,
                                    const olc::Pixel &color, const bool right) {
  for (int j = 0; j < size; ++j) {
    const int j_max = j < size / 2 ? j + 1 : size - j;
    if (right) {
      for (int i = 0; i > -j_max; --i) {
        render_engine_ref_.Draw(x + i, y + j, color);
      }
    } else {
      for (int i = 0; i < j_max; ++i) {
        render_engine_ref_.Draw(x + i, y + j, color);
      }
    }
  }
}

void Renderer::renderMenu(const MenuState &menu_state) {
  renderBackground("a-type-background");

  // Clear the top area.
  render_engine_ref_.FillRect(30, 72, 190, 50, olc::BLACK);

  render_engine_ref_.DrawString(157, 80, "OPTIONS");

  render_engine_ref_.DrawString(72, 153, "JDMFX  1357428 30");
  render_engine_ref_.DrawString(72, 169, "KORYAN 1273120 29");
  render_engine_ref_.DrawString(72, 185, "JONAS  1245200 29");

  if (menu_state.options_selected) {
    const auto color = frame_counter_ % 4 ? olc::WHITE : olc::BLACK;
    drawTriangleSelector(148, 80, 7, color, false);
    drawTriangleSelector(219, 80, 7, color, true);
    renderLevelSelector(-1);
  } else {
    renderLevelSelector(menu_state.level);
  }
}

void Renderer::renderLevelSelector(const int level) {
  constexpr Coords level_selector{51, 75};
  constexpr Rect level_selector_size{83, 35};
  constexpr Rect selector_size{16, 16};
  const olc::Pixel selector_color(252, 151, 56);

  auto get_selector_coords = [&](const int level) -> Coords {
    const int y = (level % 10 > 4) ? level_selector.y + selector_size.height : level_selector.y;
    return {(selector_size.width * (level % 5)) + level_selector.x + 1, y + 2};
  };

  // Draw the flashing selector, and then draw the levels on top with transparency.
  const auto coords = get_selector_coords(level);
  const auto color = frame_counter_ % 4 ? selector_color : olc::BLACK;
  if (level >= 0) {
    render_engine_ref_.FillRect(coords.x, coords.y, selector_size.width, selector_size.height,
                                color);
  }
  render_engine_ref_.DrawSprite(level_selector.x, level_selector.y, getSprite("levels"));

  ++frame_counter_;
}

std::vector<int> Renderer::renderOptions(
    const std::map<std::string, std::unique_ptr<OptionInterface>> &options,
    const std::vector<std::string> &option_order, const std::set<int> &spacers,
    const int left_column, const int right_column, const int first_row) {
  render_engine_ref_.DrawString(100, 17, "OPTIONS");

  std::vector<int> row_locations;
  int y_row = first_row;
  int counter = 0;
  for (const auto &name : option_order) {
    const auto &option = options.at(name);
    row_locations.push_back(y_row);
    render_engine_ref_.DrawString(left_column, y_row, option->getDisplayName());
    render_engine_ref_.DrawString(right_column, y_row, option->getSelectedOptionText());
    y_row += 10;
    if (spacers.count(counter++)) {
      y_row += 5;
    }
  }
  return row_locations;
}

void Renderer::renderSelector(const OptionState &option_state, const int column_location,
                              const std::vector<int> &row_locations) {
  const auto color = frame_counter_++ % 4 ? olc::WHITE : olc::BLACK;
  constexpr int size = 7;
  const auto &option = option_state.getSelectedOption();
  if (option.prevOptionAvailable()) {
    drawTriangleSelector(column_location, row_locations.at(option_state.selected_index), size,
                         color, true);
  }
  if (option.nextOptionAvailable()) {
    drawTriangleSelector(column_location + 40, row_locations.at(option_state.selected_index), size,
                         color, false);
  }
}

void Renderer::renderOptionScreen(const OptionState &option_state) {
  renderBackground("options-background");
  render_engine_ref_.FillRect(30, 30, 195, 180, olc::BLACK);
  const int x_left_column = 32;
  const int x_right_column = 180;
  const int y_row = 35;

  std::set<int> spacers{4, 6};
  const auto row_locations = renderOptions(option_state.options, option_state.option_order, spacers,
                                           x_left_column, x_right_column, y_row);
  renderSelector(option_state, x_right_column - 5, row_locations);
}
};  // namespace tetris_clone

