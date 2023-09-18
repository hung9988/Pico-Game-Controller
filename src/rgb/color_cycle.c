/**
 * Color cycle effect
 * @author SpeedyPotato
 **/

void ws2812b_color_cycle(uint32_t counter)
{

  for (int i = 0; i < 8; ++i)
  {
    if ((report.buttons >> 8) % 2 == 1)
    {
      int wheel_pos = -((20 * counter + i * (int)(768 / 8)) % 768);
      wheel_pos %= 768;
      if (wheel_pos < 256)
      {
        for (int j = 0; j < 32; j += 8)
        {
          ws2812b_data[i + j].r = wheel_pos * 0.5;
          ws2812b_data[i + j].g = (255 - wheel_pos) * 0.5;
          ws2812b_data[i + j].b = 0 * 0.5;
        }
      }
      else if (wheel_pos < 512)
      {

        wheel_pos -= 256;
        for (int j = 0; j < 32; j += 8)
        {
          ws2812b_data[i + j].r = (255 - wheel_pos) * 0.5;
          ws2812b_data[i + j].g = 0 * 0.5;
          ws2812b_data[i + j].b = wheel_pos * 0.5;
        }
      }
      else
      {

        wheel_pos -= 512;
        for (int j = 0; j < 32; j += 8)
        {
          ws2812b_data[i + j].r = 0 * 0.5;
          ws2812b_data[i + j].g = wheel_pos * 0.5;
          ws2812b_data[i + j].b = (255 - wheel_pos) * 0.5;
        }
      }
    }
    else
    {
      int wheel_pos = (2 * counter + i * (int)(768 / 8)) % 768;
      wheel_pos %= 768;
      if (wheel_pos < 256)
      {
        for (int j = 0; j < 32; j += 8)
        {
          ws2812b_data[i + j].r = wheel_pos * 0.5;
          ws2812b_data[i + j].g = (255 - wheel_pos) * 0.5;
          ws2812b_data[i + j].b = 0 * 0.5;
        }
      }
      else if (wheel_pos < 512)
      {

        wheel_pos -= 256;
        for (int j = 0; j < 32; j += 8)
        {
          ws2812b_data[i + j].r = (255 - wheel_pos) * 0.5;
          ws2812b_data[i + j].g = 0 * 0.5;
          ws2812b_data[i + j].b = wheel_pos * 0.5;
        }
      }
      else
      {

        wheel_pos -= 512;
        for (int j = 0; j < 32; j += 8)
        {
          ws2812b_data[i + j].r = 0 * 0.5;
          ws2812b_data[i + j].g = wheel_pos * 0.5;
          ws2812b_data[i + j].b = (255 - wheel_pos) * 0.5;
        }
      }
    }
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