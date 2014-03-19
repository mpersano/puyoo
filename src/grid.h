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
	BLOCK_EXPLODING,
	NUM_BLOCK_TYPES,
};

class grid
{
public:
	void initialize(int base_x, int base_y);
	void update(unsigned dpad_state);
	void draw(gfx::context& gfx) const;

	int get_block(int r, int c) const;
	void set_block(int r, int c, int type);
	bool is_empty(int r, int c) const;

private:
	void draw_blocks(gfx::context& gfx) const;
	void draw_background(gfx::context& gfx) const;

	void chain_explode(int r, int c, int type);
	void clear_exploding_blocks();

	bool has_hanging_blocks() const;
	void drop_hanging_blocks();
	bool find_chains();
	int find_chain_size(bool *visited, int r, int c, int type) const;

	enum state {
		STATE_PLAYER_CONTROL,
		STATE_EXPLODING_BLOCKS,
		STATE_DROPPING_BLOCKS,
	};

	void set_state(state next_state);

	class falling_block
	{
	public:
		void initialize();
	
		void draw(gfx::context& gfx, int base_x, int base_y) const;
		bool update(const grid *g, unsigned dpad_state);
		bool can_move(const grid *g, int dr, int dc) const;
		void copy_to_grid(grid *g);
	
	private:
		enum state {
			STATE_PLAYER_CONTROL,
			STATE_ROTATING,
		};

		void set_state(state next_state);

		int blocks_[2];
		int row_, col_, rotation_;
		int input_poll_tics_;
		int drop_tics_;
		state state_;
		int state_tics_;
	};

	falling_block falling_block_;

	unsigned char blocks_[GRID_ROWS*GRID_COLS];
	int base_x_, base_y_;
	state state_;
	int state_tics_;
};

void
grid_load_sprites();

#endif /* GRID_H_ */
