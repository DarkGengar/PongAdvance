/*  Author: Joel Nauschütz (DarkGengar) */
/*  TODO:   Add license header,
            Split into different functions,
            Refactor code + document code
*/

#include <tonc.h>
#include <gfx.h>

void load_gfx();
void load_objs();
void check_collision();

OBJ_ATTR obj_buffer[128];

int main() 
{
    const int STEP = 2;
    const int BALL_START_X = (SCREEN_WIDTH/2) - 4;
    const int BALL_START_Y = (SCREEN_HEIGHT/2) - 4;

    load_gfx();

    oam_init(obj_buffer, 128);
    REG_BG0CNT = BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    int sign_x = -1;
    int sign_y = 1;
    int ply_mov = 0;

    int ply_x = 0, ply_y = 0;
    int eny_x = SCREEN_WIDTH-9, eny_y = SCREEN_HEIGHT-33;
    int ball_x = BALL_START_X, ball_y = BALL_START_Y;

    u32 ply_tid = 0, ball_tid = 4, pb = 0;
    OBJ_ATTR *player = &obj_buffer[0];
    OBJ_ATTR *enemy = &obj_buffer[1];
    OBJ_ATTR *ball = &obj_buffer[2];

    /* --- Load And Configure Sprites --- */
    obj_set_attr(
        player,
        ATTR0_TALL,
        ATTR1_SIZE_8x32,
        ATTR2_PALBANK(pb) | ply_tid);

    obj_set_attr(
        ball,
        ATTR0_SQUARE,
        ATTR1_SIZE_8x8,
        ATTR2_PALBANK(pb) | ball_tid);

    *enemy = *player;

    obj_set_pos(player, ply_x, ply_y);
    obj_set_pos(enemy, eny_x, eny_y);
    obj_set_pos(ball, ball_x, ball_y);
    
    while(TRUE) 
    {
        vid_vsync();
        key_poll();

        /* --- Ball Handling --- */
        if (ball_y < 0 || ball_y > (SCREEN_HEIGHT - 7)) 
        {
            sign_y *= -1;
        }

        if (ball_x < 0)
        {
            ball_x = BALL_START_X;
        }
        else if (ball_x > (SCREEN_WIDTH - 7))
        {
            ball_x = BALL_START_X;
        }
        ball_x += sign_x * 1;
        ball_y += sign_y * 1;
    
        /* --- Player Movement --- */
        ply_mov = key_tri_vert();
        ply_y += STEP * ply_mov;
        ply_y = clamp(ply_y, 0, SCREEN_HEIGHT-32);

        /* --- Enemy Movement --- */
        if (sign_x == 1)
        {
            if ((ball_y + 7) < eny_y)
            {
                eny_y += STEP * -1;
            } /* upper enemy bound */
            else if (ball_y > (eny_y + 31))
            {
                eny_y += STEP * 1;
            } /* lower enemy bouind */
        }

        /* --- Collision Detection --- */
        if ((ply_x + 7) > ball_x && ply_y < (ball_y + 7) && (ply_y + 31) > ball_y)
        {
            sign_x = 1;
            sign_y = ply_mov;

            ball_x += sign_x * 1;
            ball_y += sign_y * 1;
        } /* player-ball collision */
        else if (eny_x < (ball_x + 7) && eny_y  < (ball_y + 7) && (eny_y + 31) > ball_y)
        {
            sign_x = -1;
            sign_y *= -1;

            ball_x += sign_x * 1;
            ball_y += sign_y * 1;
        } /* enemy-ball collision */

        obj_set_pos(player, ply_x, ply_y);
        obj_set_pos(ball, ball_x, ball_y);
        obj_set_pos(enemy, eny_x, eny_y);
        oam_copy(oam_mem, obj_buffer, 3);
    }

    return 0;
}

void load_gfx() 
{
    /* --- Background --- */
    memcpy32(&tile_mem[0][0], pong_bgTiles, pong_bgTilesLen);
    memcpy32(&se_mem[31][0], pong_bgMap, pong_bgMapLen);
    memcpy32(pal_bg_mem, pong_bgPal, pong_bgPalLen);

    /* --- Sprites --- */
    memcpy32(&tile_mem[4][0], pong_player_sprTiles, pong_player_sprTilesLen);
    memcpy32(pal_obj_mem, pong_player_sprPal, pong_player_sprPalLen);
    memcpy32(&tile_mem[4][4], pong_ball_sprTiles, pong_ball_sprTilesLen);
}

void check_collision()
{

}