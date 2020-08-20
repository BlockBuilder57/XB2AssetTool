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
			/**
			 * Fired on slider change.
			 */
			virtual void sliderChange(SliderChange change);

			/**
			 * Override of QSlider::event() so we can capture hover events
			 */
			virtual bool event(QEvent* event);

		   private:
			/**
			 * Show the value by creating a tooltip.
			 */
			void ShowValue();
		};

	} // namespace ui
} // namespace xb2at