#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include <QMainWindow>
#include "mask.cpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_load_img_btn_clicked();
  void on_red_clicked();
  void on_green_clicked();
  void on_blue_clicked();
  void on_grayscale_btn_clicked();
  void on_previous_img_btn_clicked();
  void on_choose_img_btn_clicked();
  void on_binarization_btn_clicked();
  void on_cross_clicked();
  void on_disk_clicked();
  void on_rectangle_clicked();
  void on_rhomb_clicked();
  void on_ring_clicked();
  void on_apply_primitive_size_clicked();
  void on_border_selection_clicked();
  void on_erosion_clicked();
  void on_opening_clicked();
  void on_closure_clicked();
  void on_conditional_buildup_clicked();
  void on_dilatation_clicked();

private:
  Ui::MainWindow *ui;

  // для обработки изображения
  QImage capture_image(const QPixmap* img_pix);
  void process_image(QImage &img, const std::string& color);
  void set_image(QImage &img, QLabel *area);
  void full_process_img(const std::string& color);

  // Отображение и получение данных примитива
  QImage image_fitting(QImage &image, int image_size);
  QImage image_fitting(QImage &image, int width, int height);
  void get_primitive_values();
  void get_primitive_sizes(int &half_width, int &half_height,
                           int &width, int &height);

  QImage create_clear_image(int width, int height);
  std::vector<Mask> create_mask();

  int primitive_size, rect_primitive_size_2; // размер примитива (2ой для прямоугольника)
  QImage primitive; // примитив
  QString path; // путь к изображению
  std::vector<QImage> previous_state; // сохранение предыдущих состояний
};
#endif // MAINWINDOW_H
