#ifndef GRID_H_
#define GRID_H_

#include "gfx_context.h"

enum {
	BLOCK_SIZE = 16,
	GRID_ROWS = 12,
	GRID_COLS = 6,
};

enum block_type {
	BLOCK_EMPTY,
	BLOCK_CYAN,
	BLOCK_RED,
	BLOCK_BLUE,
	BLOCK_GREEN,
	BLOCK_PURPLE,
	BLOCK_YELLOW,
	BLOCK_JAMA,
	BLOCK_EXPLODING,

	NUM_BLOCK_TYPES = BLOCK_YELLOW - BLOCK_CYAN + 1,
};

class falling_block;

class grid
{
public:
	void initialize(int base_x, int base_y, grid *opponent, bool human_control);

	void reset();
	void update(unsigned dpad_state);
	void draw(gfx::context& gfx) const;

	int get_block(int r, int c) const;
	void set_block(int r, int c, int type);
	bool is_empty(int r, int c) const;

	void add_jama(int num_jama);

private:
	void draw_blocks(gfx::context& gfx) const;
	void draw_dropping_jama(gfx::context& gfx) const;
	void draw_background(gfx::context& gfx) const;
	void draw_hud(gfx::context& gfx) const;

	void chain_explode(int r, int c, int type);
	void clear_exploding_blocks();

	void drop_jama();
	void copy_jama_to_grid();

	bool has_hanging_blocks() const;
	void drop_hanging_blocks();
	bool find_chains();
	int find_chain_size(bool *visited, int r, int c, int type) const;

	int get_col_height(int r) const;

	enum state {
		STATE_PLAYER_CONTROL,
		STATE_EXPLODING_BLOCKS,
		STATE_DROPPING_BLOCKS,
		STATE_DROPPING_JAMA,
	};

	void set_state(state next_state);

	falling_block *falling_block_;
	unsigned char blocks_[GRID_ROWS*GRID_COLS];
	int base_x_, base_y_;
	state state_;
	int state_tics_;
	int jama_to_drop_, jama_drop_tics_;
	int dropping_jama_[GRID_COLS];
	int combo_size_;
	grid *opponent_;
};

void
grid_init_resources();

#endif /* GRID_H_ */
