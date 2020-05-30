#include "ShowValueSlider.h"
#include <QStyleOptionSlider>
#include <QToolTip>

namespace xb2at {
namespace ui {

	ShowValueSlider::ShowValueSlider(QWidget* parent)
		: QSlider(parent) {
		// Request hover events
		setAttribute(Qt::WA_Hover, true);
	}

	ShowValueSlider::ShowValueSlider(Qt::Orientation orientation, QWidget* parent)
		: QSlider(orientation, parent) {
	}

	void ShowValueSlider::sliderChange(QAbstractSlider::SliderChange change) {
		QSlider::sliderChange(change);

		if(change == QAbstractSlider::SliderValueChange) {
			ShowValue();
		}
	}

	bool ShowValueSlider::event(QEvent* event) {
		switch (event->type()) {
		case QEvent::HoverEnter:
			// Only show the value tooltip if the slider is enabled (the user is allowed to change it). 
			if(isEnabled())
				ShowValue();
			break;

		case QEvent::HoverLeave:
			break;

		default:
			break;
		}

		return QSlider::event(event);
	}

	void ShowValueSlider::ShowValue() {
		QStyleOptionSlider opt;
		initStyleOption(&opt);

		QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
		QPoint topLeftCorner = sr.topLeft();

		QToolTip::showText(mapToGlobal(QPoint(topLeftCorner.x() - 7, topLeftCorner.y() - 40)), QString::number(value()), this);
	}

}
}