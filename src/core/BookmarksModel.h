/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2013 - 2014 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
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

#ifndef OTTER_BOOKMARKSMODEL_H
#define OTTER_BOOKMARKSMODEL_H

#include <QtGui/QStandardItemModel>

namespace Otter
{

class BookmarksModel : public QStandardItemModel
{
	Q_OBJECT

public:
	enum BookmarksRole
	{
		BookmarkTitleRole = Qt::DisplayRole,
		BookmarkDescriptionRole = Qt::ToolTipRole,
		BookmarkTypeRole = Qt::UserRole,
		BookmarkUrlRole = (Qt::UserRole + 1),
		BookmarkKeywordRole = (Qt::UserRole + 2),
		BookmarkTimeAddedRole = (Qt::UserRole + 3),
		BookmarkTimeModifiedRole = (Qt::UserRole + 4),
		BookmarkTimeVisitedRole = (Qt::UserRole + 5),
		BookmarkVisitsRole = (Qt::UserRole + 6)
	};

	explicit BookmarksModel(QObject *parent = NULL);

protected slots:

};

}

#endif