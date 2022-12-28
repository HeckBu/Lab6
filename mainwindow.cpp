#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "binarization.cpp"
#include "QWidget"
#include "QImage"
#include "QPixmap"
#include "QFileDialog"
#include "QPainter"
#include "QRgb"
#include <QDebug>
#include <QMessageBox>
#include <vector>
#include <cmath>

struct Mask {
  int x;
  int y;
};

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_load_img_btn_clicked() {
  this->path = QFileDialog::getOpenFileName(0, "Open Dialog", "", "* .png * .jpg * .bmp");
  QPixmap img(this->path); // получение изображения

  img = img.scaled(ui->loaded_image->width(),
                   ui->loaded_image->height(),
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation);  // функция масштаба

  ui->loaded_image->setPixmap(img); // вывод начального изображения
}

QImage MainWindow::capture_image(const QPixmap* img_pix) {
  // Проверка на наличие загруженного изображения
  if ( img_pix != nullptr ) {
    const QPixmap* pix = img_pix;
    return QImage(pix->toImage());
  }
  return QImage(":/assets/no_image.jpg").scaled(ui->loaded_image->size());
}

void MainWindow::set_image(QImage &img, QLabel *area){
  QPixmap pix = QPixmap::fromImage(img);
  area->setPixmap(pix);
}

void MainWindow::process_image(QImage &img, const std::string& color) {
  QColor rgb;

  if(color == "red") {
    for (int i = 0; i < img.width(); ++i) {
      for (int j = 0; j < img.height(); ++j) {
        rgb = img.pixel(i,j);
        rgb.setGreen(0);
        rgb.setBlue(0);
        img.setPixelColor(i,j,rgb);
      }
    }
  } else if (color == "green") {
    for (int i = 0; i < img.width(); ++i) {
      for (int j = 0; j < img.height(); ++j) {
        rgb = img.pixel(i,j);
        rgb.setRed(0);
        rgb.setBlue(0);
        img.setPixelColor(i,j,rgb);
      }
    }
  } else if (color == "blue") {
    for (int i = 0; i < img.width(); ++i) {
      for (int j = 0; j < img.height(); ++j) {
        rgb = img.pixel(i,j);
        rgb.setRed(0);
        rgb.setGreen(0);
        img.setPixelColor(i,j,rgb);
      }
    }
  } else if (color == "grey") {
    float color_gray = 0;
    for (int i = 0; i < img.width(); ++i){
      for (int j = 0; j < img.height(); ++j){
        rgb = img.pixel(i,j);
        color_gray = rgb.red()*0.299 + rgb.green()*0.587 + rgb.blue()*0.114;
        rgb.setRed(color_gray);
        rgb.setGreen(color_gray);
        rgb.setBlue(color_gray);
        img.setPixelColor(i,j,rgb);
      }
    }
  }
}

void MainWindow::full_process_img(const std::string& color) {
  QImage image = capture_image(ui->changeable_image->pixmap());
  this->previous_state.push_back(image);
  process_image(image, color);
  set_image(image, ui->changeable_image);
}

void MainWindow::on_red_clicked() {
  full_process_img("red");
}

void MainWindow::on_green_clicked() {
  full_process_img("green");
}

void MainWindow::on_blue_clicked() {
  full_process_img("blue");
}

void MainWindow::on_grayscale_btn_clicked() {
  full_process_img("grey");
}


void MainWindow::on_previous_img_btn_clicked() {
  if (this->previous_state.size() > 0) {
    QImage image = this->previous_state[this->previous_state.size() - 1];
    this->previous_state.pop_back();
    set_image(image, ui->changeable_image);
  }
}

void MainWindow::on_choose_img_btn_clicked() {
  QImage image = capture_image(ui->loaded_image->pixmap());
  this->previous_state.push_back(image);
  set_image(image, ui->changeable_image);
}

void MainWindow::on_binarization_btn_clicked() {
  QImage image = capture_image(ui->changeable_image->pixmap());
  breadly_binarization(image);
  this->previous_state.push_back(image);
  set_image(image, ui->changeable_image);
}

QImage MainWindow::image_fitting(QImage &image, int image_size) {
  // обрезаем лишнее из примитива 9х9
  QRect rect(0, 0, image_size, image_size);
  QImage cropped = image.copy(rect);
  // записываем в image примитив
  image = cropped;

  // масштабируем под окно примитива
  cropped = cropped.scaled(
        ui->primitive_image->width(),
        ui->primitive_image->height(),
        Qt::KeepAspectRatioByExpanding,
        Qt::FastTransformation);

  return cropped;
}

QImage MainWindow::image_fitting(QImage &image, int width, int height) {
  // обрезаем лишнее из примитива 9х9
  QRect rect(0, 0, width, height);
  QImage cropped = image.copy(rect);
  // записываем в image примитив
  image = cropped;

  // масштабируем под окно примитива
  cropped = cropped.scaled(
        ui->primitive_image->width(),
        ui->primitive_image->height(),
        Qt::KeepAspectRatio,
        Qt::FastTransformation);

  return cropped;
}

void MainWindow::get_primitive_values() {
  this->primitive_size = ui->primitive_size->text().toInt();
  this->primitive = QImage(":/assets/white.jpg");
}

// Примитивы

void MainWindow::on_cross_clicked() { // крест
  get_primitive_values();
  QColor rgb;

  if (this->primitive_size >= 3 && this->primitive_size <= 9) {

    if (this->primitive_size % 2 == 1) {
      int middle_pix = floor(this->primitive_size/2); // центр. линия
      for (int i = 0; i < this->primitive_size; ++i) {
        for (int j = 0; j < this->primitive_size; ++j) {
          rgb = this->primitive.pixel(i,j);
          if (i == middle_pix || j == middle_pix) {
            rgb.setRgb(0,0,0); // черный
          } else {
            rgb.setRgb(255,255,255); // белый
          }
          this->primitive.setPixelColor(i,j,rgb);
        }
      }
    } else {
      int middle_pixels[2];
      middle_pixels[0] = this->primitive_size/2; // центр. линия 1
      middle_pixels[1] = middle_pixels[0] - 1;  // центр. линия 2
      for (int i = 0; i < this->primitive_size; ++i) {
        for (int j = 0; j < this->primitive_size; ++j) {
          rgb = this->primitive.pixel(i,j);
          if (i == middle_pixels[0] || j == middle_pixels[0] ||
              i == middle_pixels[1] || j == middle_pixels[1]) {
            rgb.setRgb(0,0,0); // черный
          } else {
            rgb.setRgb(255,255,255); // белый
          }
          this->primitive.setPixelColor(i,j,rgb);
        }
      }
    }

    QImage image = image_fitting(this->primitive, this->primitive_size);
    set_image(image, ui->primitive_image);
  }
}

void MainWindow::on_disk_clicked() { // диск
  get_primitive_values();
  QColor rgb;

  if (this->primitive_size >= 3 && this->primitive_size <= 9) {

    for (int i = 0; i < this->primitive_size; ++i) {
      for (int j = 0; j < this->primitive_size; ++j) {
        rgb = this->primitive.pixel(i,j);
        // форма диска достигается без крайних пикселей
        if ( (i == 0 && j == 0) ||
             (i == 0 && j == this->primitive_size - 1) ||
             (i == this->primitive_size - 1 && j == 0) ||
             (i == this->primitive_size - 1 && j == this->primitive_size - 1) ) {
          rgb.setRgb(255,255,255); // белый
        } else {
          rgb.setRgb(0,0,0); // черный
        }
        this->primitive.setPixelColor(i,j,rgb);
      }
    }

    QImage image = image_fitting(this->primitive, this->primitive_size);
    set_image(image, ui->primitive_image);
  }
}


void MainWindow::on_rectangle_clicked() { // прямоугольник
  this->primitive_size = ui->primitive_size->text().toInt();
  this->rect_primitive_size_2 = ui->rect_primitive_size_2->text().toInt();
  this->primitive = QImage(":/assets/white.jpg");
  QColor rgb;

  if (this->primitive_size >= 1 && this->primitive_size <= 9 &&
      this->rect_primitive_size_2 >= 1 && this->rect_primitive_size_2 <= 9) {

    for (int i = 0; i < this->primitive_size; ++i) {
      for (int j = 0; j < this->rect_primitive_size_2; ++j) {
        rgb = this->primitive.pixel(i,j);
        rgb.setRgb(0,0,0); // черный
        this->primitive.setPixelColor(i,j,rgb);
      }
    }

    QImage image = image_fitting(this->primitive,
           this->primitive_size, this->rect_primitive_size_2);
    set_image(image, ui->primitive_image);
  }
}

void MainWindow::on_rhomb_clicked() { // ромб
  get_primitive_values();
  QColor rgb;
  int ps, ps_left_up;

  if (this->primitive_size % 2 == 0) {
      ps = (this->primitive_size)/2;
      ps_left_up = ps-1;
  } else {
      ps = floor((this->primitive_size)/2);
      ps_left_up = ps;
  }

  if (this->primitive_size >= 3 && this->primitive_size <= 9) {
     for (int i = 0; i < this->primitive_size; ++i) {
      for (int j = 0; j < this->primitive_size; ++j) {
        rgb = this->primitive.pixel(i,j);
        // форма диска достигается без крайних пикселей
        if ( (i+j < ps_left_up) || (i-j > ps) ||
             (i+j >= this->primitive_size+ps) || (j-i > ps) ) {
          rgb.setRgb(255,255,255); // белый
        } else {
          rgb.setRgb(0,0,0); // черный
        }
        this->primitive.setPixelColor(i,j,rgb);
      }
    }
    QImage image = image_fitting(this->primitive, this->primitive_size);
    set_image(image, ui->primitive_image);
  }
}

void MainWindow::on_ring_clicked() { // кольцо
  get_primitive_values();
  int ps = this->primitive_size - 1;
  QColor rgb;


  if (this->primitive_size >= 3 && this->primitive_size <= 9) {

    for (int i = 0; i < this->primitive_size; ++i) {
      for (int j = 0; j < this->primitive_size; ++j) {
        rgb = this->primitive.pixel(i,j);
        // форма диска достигается без крайних пикселей
        if ( ((i == 0  && j !=  0) && (i == 0  && j != ps)) ||
             ((i == ps && j !=  0) && (i == ps && j != ps)) ||
             ((i != 0  && j ==  0) && (i != ps && j ==  0)) ||
             ((i != 0  && j == ps) && (i != ps && j == ps)) ) {
          rgb.setRgb(0,0,0); // черный
        } else {
          rgb.setRgb(255,255,255); // белый
        }
        this->primitive.setPixelColor(i,j,rgb);
      }
    }

    QImage image = image_fitting(this->primitive, this->primitive_size);
    set_image(image, ui->primitive_image);
  }
}

// кнопка применить размер примитива
void MainWindow::on_apply_primitive_size_clicked() {
  if (ui->rectangle->isChecked())
    on_rectangle_clicked();
  if (ui->cross->isChecked())
    on_cross_clicked();
  if (ui->disk->isChecked())
    on_disk_clicked();
  if (ui->rhomb->isChecked())
    on_rhomb_clicked();
  if (ui->ring->isChecked())
    on_ring_clicked();
}

// выделение границ
void MainWindow::on_border_selection_clicked() {

}

// эрозия
void MainWindow::on_erosion_clicked() {
  // захватываем изображение
  QImage image = capture_image(ui->changeable_image->pixmap());
  // если был изменен размер примитива пересчитываем его
  on_apply_primitive_size_clicked();

  // вычисление [полуширины, ширины, полувысоты, высоты] примитива
  int half_width, half_height, width, height;
  if (ui->rectangle->isChecked()) {
    half_width =  static_cast<int>(floor(this->primitive_size/2));
    width = this->primitive_size;
    half_height =  static_cast<int>(floor(this->rect_primitive_size_2/2));
    height = this->rect_primitive_size_2;
    // для симметричных примитивов
  } else {
    half_width = static_cast<int>(floor(this->primitive_size/2));
    width = this->primitive_size;
    half_height = half_width;
    height = width;
  }

  std::vector<Mask> mask;
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      if (this->primitive.pixelColor(i, j) == QColor(0,0,0)) {
        mask.push_back({i-half_width, j-half_height});
      }
    }
  }

  // для отладки
//  for (auto [x,y] : mask) {
//    qDebug() << "x: " << x << "y: " << y << '\n';
//  }

  // создаем и обнуляем изображение для записи
  QImage erosion_image(image.width() - 1,
                          image.height() - 1,
                          QImage::Format_RGB32);
  for (int i = 0; i < erosion_image.width() - 1; ++i) {
    for (int j = 0; j < erosion_image.height() - 1; ++j) {
      erosion_image.pixel(i, j);
      erosion_image.setPixelColor(i,j, QColor(0,0,0));
    }
  }


  for (int i = half_width; i < image.width() - 1 - half_width; ++i) {
    for (int j = half_height; j < image.height() - 1 - half_height; ++j) {
      bool flag = false;

      for (int x = 0; x < width - 1; ++x) {
        for (int y = 0; y < height - 1; ++y) {
          if (this->primitive.pixel(x, y) >
              image.pixel(i+x-half_width, j+y-half_height)) {
            flag = true;
            x = width;
            break;
          }
        }
      }
      if (flag) {
        for (auto [m_x, m_y] : mask) {
          erosion_image.pixel(i+m_x, j+m_y);
          erosion_image.setPixelColor(i+m_x,j+m_y, QColor(255,255,255));
        }
      }
    }
  }


  this->previous_state.push_back(erosion_image);
  set_image(erosion_image, ui->changeable_image);
}

// размыкание
void MainWindow::on_opening_clicked() {

}

// замыкание
void MainWindow::on_closure_clicked() {

}

// условное наращивание
void MainWindow::on_conditional_buildup_clicked() {

}

// дилатация
void MainWindow::on_dilatation_clicked() {

}

