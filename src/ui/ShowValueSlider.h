#pragma once
#include <QEvent>
#include <QSlider>


namespace xb2at {
namespace ui {

	class ShowValueSlider : public QSlider {
		Q_OBJECT
	public:
		explicit ShowValueSlider(QWidget* parent = nullptr);
		explicit ShowValueSlider(Qt::Orientation orientation, QWidget* parent = nullptr);

	protected:
		virtual void sliderChange(SliderChange change);
		virtual bool event(QEvent* event);

	private:
		void ShowValue();
	};

}
}