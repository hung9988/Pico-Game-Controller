/**
 * @author 4yn, SpeedyPotato
 * Turbocharger chasing laser effect
 *
 * Move 2 lighting areas around the controller depending on knob input.
 *
 * For each knob, calculate every 5 ms:
 * - Add any knob delta to a counter
 * - Clamp counter to some "maximum speed"
 * - If counter is far enough from 0, knob is moving
 * - If counter is too near 0 and has been there for a while, fade out
 * - Move the lighting area in the correct direction at a constant speed
 * - Decay the counter
 *
 * Curent values are tuned for
 * - Lights sampled at 200 Hz
 * - 0.1 rotations for lights to activate
 * - Lighting areas take 0.75s to make one full rotation
 * - Movement takes 0.5s to decay to stop
 * - Fade out takes another 0.2s samples to disappear
 *
 * LEDs are positioned as:
 * - 0, 1 as dummy leds on top of controller
 * - 2-6 LEDs on right edge, top to bottom
 * - 7-9 as dummy leds on bottom of controller
 * - 10-14 LEDs on left edge, bottom to top
 * - 15 as dummy led on top of controller
 *
 * Lighting areas start at position 0 and will light up the 3 nearest LEDs.
 * By strategically positioning led 0 at the top, this avoids lighting areas
 * from suddenly appeaering.
 **/

#define TURBO_LIGHTS_CLAMP 0.1f
#define TURBO_LIGHTS_THRESHOLD 0.05f
#define TURBO_LIGHTS_DECAY 0.0047f
#define TURBO_LIGHTS_VEL 0.5f
#define TURBO_LIGHTS_FADE 5
#define TURBO_LIGHTS_FADE_VEL 0.2f
#define START_LEFT_POS 0
#define START_RIGHT_POS 7
#define STRIP_SIZE 4

int i_clamp(int d, int min, int max)
{
  const int t = d < min ? min : d;
  return t > max ? max : t;
}

float f_clamp(float d, float min, float max)
{
  const float t = d < min ? min : d;
  return t > max ? max : t;
}

float f_one_mod(float d)
{
  const float t = d < 0 ? START_RIGHT_POS + d : d;
  return t > START_RIGHT_POS ? t - START_RIGHT_POS : t;
}

float f_abs(float d)
{
  return d < 0 ? -d : d;
}

uint32_t turbo_prev_enc_val[ENC_GPIO_SIZE];
float turbo_cur_enc_val[ENC_GPIO_SIZE];
float turbo_lights_pos[ENC_GPIO_SIZE] = {START_LEFT_POS, START_RIGHT_POS};
float turbo_lights_brightness[ENC_GPIO_SIZE];
int turbo_lights_idle[ENC_GPIO_SIZE];
RGB_t ws2812b_data[(START_RIGHT_POS + 1) * STRIP_SIZE] = {0};
void custom_turbocharger(uint32_t counter)
{

  for (int i = 0; i < ENC_GPIO_SIZE; i++)
  {
    int enc_delta = (enc_val[i] - turbo_prev_enc_val[i]) * (ENC_REV[i] ? 1 : -1);
    turbo_prev_enc_val[i] = enc_val[i];
    turbo_cur_enc_val[i] = f_clamp(turbo_cur_enc_val[i] + (float)(enc_delta) / ENC_PULSE, -TURBO_LIGHTS_CLAMP, TURBO_LIGHTS_CLAMP);

    if (turbo_cur_enc_val[i] < -TURBO_LIGHTS_THRESHOLD)
    {
      turbo_lights_idle[i] = 0;
      turbo_lights_pos[i] -= TURBO_LIGHTS_VEL;
      turbo_lights_brightness[i] = 1.0f;
    }
    else if (turbo_cur_enc_val[i] > TURBO_LIGHTS_THRESHOLD)
    {
      turbo_lights_idle[i] = 0;
      turbo_lights_pos[i] += TURBO_LIGHTS_VEL;
      turbo_lights_brightness[i] = 1.0f;
    }
    else
    {
      turbo_lights_idle[i]++;
      if (turbo_lights_idle[i] > TURBO_LIGHTS_FADE)
      {
        if (i == 0)
        {
          turbo_lights_pos[i] = START_LEFT_POS;
        }
        else
        {
          turbo_lights_pos[i] = START_RIGHT_POS;
        }
      }
      else
      {
        turbo_lights_brightness[i] = f_clamp(turbo_lights_brightness[i] - TURBO_LIGHTS_FADE_VEL, 0.0f, 1.0f);
      }
    }

    turbo_lights_pos[i] = f_one_mod(turbo_lights_pos[i]);

    if (turbo_cur_enc_val[i] < -TURBO_LIGHTS_DECAY)
    {
      turbo_cur_enc_val[i] += TURBO_LIGHTS_DECAY;
    }
    else if (turbo_cur_enc_val[i] > TURBO_LIGHTS_DECAY)
    {
      turbo_cur_enc_val[i] -= TURBO_LIGHTS_DECAY;
    }
  }

  for (int i = 0; i < START_RIGHT_POS + 1; i++)
  {
    float pos = i;
    float l_strength = (1.0f - f_clamp(f_abs(turbo_lights_pos[0] - pos), 0.0f, 2.0f) / 2) * turbo_lights_brightness[0];
    float r_strength = (1.0f - f_clamp(f_abs(turbo_lights_pos[1] - pos), 0.0f, 2.0f) / 2) * turbo_lights_brightness[1];
    for (int j = i; j < (7 + 1) * STRIP_SIZE; j += (7 + 1))
    {
      ws2812b_data[j].r = i_clamp(l_strength * 70 + r_strength * 250, 0, 255);
      ws2812b_data[j].g = i_clamp(l_strength * 230 + r_strength * 60, 0, 255);
      ws2812b_data[j].b = i_clamp(l_strength * 250 + r_strength * 200, 0, 255);
    }
  }
  if ((report.buttons >> 4) % 2 == 1)
  {
    ws2812b_data[1].r = 62;
    ws2812b_data[1].g = 100;
    ws2812b_data[1].b = 0;
    ws2812b_data[0].r = 62;
    ws2812b_data[0].g = 100;
    ws2812b_data[0].b = 0;
  }
  if ((report.buttons >> 2) % 2 == 1)
  {
    ws2812b_data[2].r = 62;
    ws2812b_data[2].g = 100;
    ws2812b_data[2].b = 0;
    ws2812b_data[3].r = 62;
    ws2812b_data[3].g = 100;
    ws2812b_data[3].b = 0;
  }
  if ((report.buttons >> 1) % 2 == 1)
  {
    ws2812b_data[4].r = 62;
    ws2812b_data[4].g = 100;
    ws2812b_data[4].b = 0;
    ws2812b_data[5].r = 62;
    ws2812b_data[5].g = 100;
    ws2812b_data[5].b = 0;
  }
  if ((report.buttons >> 0) % 2 == 1)
  {
    ws2812b_data[6].r = 62;
    ws2812b_data[6].g = 100;
    ws2812b_data[6].b = 0;
    ws2812b_data[7].r = 62;
    ws2812b_data[7].g = 100;
    ws2812b_data[7].b = 0;
  }
  if ((report.buttons >> 3) % 2 == 1)
  {
    ws2812b_data[8].r = 255;
    ws2812b_data[8].g = 15;
    ws2812b_data[8].b = 15;
    ws2812b_data[9].r = 255;
    ws2812b_data[9].g = 15;
    ws2812b_data[9].b = 15;
  }
  if ((report.buttons >> 5) % 2 == 1)
  {
    ws2812b_data[12].r = 255;
    ws2812b_data[12].g = 15;
    ws2812b_data[12].b = 15;
    ws2812b_data[13].r = 255;
    ws2812b_data[13].g = 15;
    ws2812b_data[13].b = 15;
  }
  if ((report.buttons >> 8) % 2 == 1)
  {
    ws2812b_data[16].r = 255;
    ws2812b_data[16].g = 230;
    ws2812b_data[16].b = 0;
    ws2812b_data[15].r = 255;
    ws2812b_data[15].g = 230;
    ws2812b_data[15].b = 0;
  }
  for (int i = 0; i < 8; i++)
  {
    put_pixel(
        urgb_u32(ws2812b_data[i].r, ws2812b_data[i].g, ws2812b_data[i].b));
  }
  for (int i = 13; i > 7; i--)
  {
    put_pixel(
        urgb_u32(ws2812b_data[i].r, ws2812b_data[i].g, ws2812b_data[i].b));
  }

  for (int i = 18; i < 24; i++)
  {
    put_pixel(
        urgb_u32(ws2812b_data[i].r, ws2812b_data[i].g, ws2812b_data[i].b));
  }
  for (int i = 28; i > 26; i--)
  {
    put_pixel(
        urgb_u32(ws2812b_data[i].r, ws2812b_data[i].g, ws2812b_data[i].b));
  }
}
