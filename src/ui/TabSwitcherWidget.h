/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2015 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#ifndef OTTER_TABSWITCHERWIDGET_H
#define OTTER_TABSWITCHERWIDGET_H

#include <QtGui/QStandardItemModel>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>

namespace Otter
{

class Window;
class WindowsManager;

class TabSwitcherWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TabSwitcherWidget(WindowsManager *manager, QWidget *parent = NULL);

	void show(bool triggeredByAction);
	void accept();
	void selectTab(bool next);
	bool eventFilter(QObject *object, QEvent *event);

protected:
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	QList<QStandardItem*> createRow(Window *window) const;
	int findRow(qint64 identifier) const;

protected slots:
	void currentTabChanged(const QModelIndex &index);
	void tabAdded(qint64 identifier);
	void tabRemoved(qint64 identifier);
	void setTitle(const QString &title);
	void setIcon(const QIcon &icon);

private:
	WindowsManager *m_windowsManager;
	QStandardItemModel *m_model;
	QFrame *m_frame;
	QListView *m_tabsView;
	QLabel *m_previewLabel;
	QMovie *m_loadingMovie;
	bool m_triggeredByAction;
};

}

#endif
