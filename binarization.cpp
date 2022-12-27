#include <QImage>
#include "QColor"

QImage breadly_binarization(QImage &image) {
  QColor rgb;

  double sum_red, sum_green, sum_blue;
  double red_new, green_new, blue_new;
  double bean_red[image.width()][image.height()];
  double bean_green[image.width()][image.height()];
  double bean_blue[image.width()][image.height()];

  for (int i=0; i<image.width(); ++i) {
    for (int j=0; j<image.height(); ++j) {
      bean_red[i][j]=0;
      bean_green[i][j]=0;
      bean_blue[i][j]=0;
    }
  }

  //перевод в интегральное изображение
  for (int j=0; j<image.height(); ++j) {
    sum_red = 0;
    sum_green = 0;
    sum_blue = 0;

    for (int i=0; i<image.width(); ++i) {
      rgb = image.pixel(i,j);
      double red_dot = rgb.red();
      double green_dot = rgb.green();
      double blue_dot = rgb.blue();

      sum_red += red_dot/255;
      sum_green += green_dot/255;
      sum_blue += blue_dot/255;
      if (j >= 1 && i >= 1) //все случаи
      {
        bean_red[i][j] = red_dot/255 + bean_red[i-1][j] + bean_red[i][j-1] - bean_red[i-1][j-1];
        bean_green[i][j] = green_dot/255 + bean_green[i-1][j] + bean_green[i][j-1] - bean_green[i-1][j-1];
        bean_blue[i][j] = blue_dot/255 + bean_blue[i-1][j] + bean_blue[i][j-1] - bean_blue[i-1][j-1];
      }
      if (j==0)  // случай первой строки
      {
        bean_red[i][j] = red_dot/255 + bean_red[i-1][j];
        bean_green[i][j] = green_dot/255 + bean_green[i-1][j];
        bean_blue[i][j] = blue_dot/255 + bean_blue[i-1][j];
      }
      if (i==0&&j>=1) // случай первого столбца
      {
        bean_red[i][j] = red_dot/255 + bean_red[i][j-1];
        bean_green[i][j] = green_dot/255 + bean_green[i][j-1];
        bean_blue[i][j] = blue_dot/255 + bean_blue[i][j-1];
      }
    }
  }

  double t = 0.1; // Параметр Бредли
  double d = (image.height()/8), s2 = d/2;

  for(int j=0; j<image.height(); ++j) {
    for(int i=0; i<image.width(); ++i) {
      rgb = image.pixel(i,j);
      int x1=i-s2,  x2=i+s2,  y1=j-s2,  y2=j+s2; // координаты прямоугольника
      if (x1 < 0) { //проверка на край поля
        x1 = 0;
      }
      if (x2 >= image.width()) {
        x2 = image.width()-1;
      }
      if (y1 < 0) {
        y1 = 0;
      }
      if (y2 >= image.height()) {
        y2 = image.height()-1;
      }

      double count = (x2-x1)*(y2-y1);  // количество ячеек в области
      sum_red = bean_red[x2][y2] - bean_red[x2][y1] - bean_red[x1][y2] + bean_red[x1][y1]; // суммарная интенсивность
      sum_green = bean_green[x2][y2] - bean_green[x2][y1] - bean_green[x1][y2] + bean_green[x1][y1]; // суммарная интенсивность
      sum_blue = bean_blue[x2][y2] - bean_blue[x2][y1] - bean_blue[x1][y2] + bean_blue[x1][y1]; // суммарная интенсивность

      if ((float)rgb.red()/255< (sum_red*(1.0+t)/count)) {
        red_new = 0;
      } else {
        red_new = 255;
      }
      if ((float)rgb.green()/255 < (sum_green*(1.0+t)/count)) {
        green_new = 0;
      } else {
        green_new = 255;
      }
      if ((float)rgb.blue()/255 < (sum_blue*(1.0+t)/count)) {
        blue_new = 0;
      } else {
        blue_new = 255;
      }

      rgb.setRgb(red_new, green_new, blue_new);
      image.setPixelColor(i,j,rgb);
    }
  }

  return image;
}
