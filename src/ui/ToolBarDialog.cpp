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

#include "ToolBarDialog.h"
#include "../core/ActionsManager.h"
#include "../core/BookmarksManager.h"
#include "../core/BookmarksModel.h"
#include "../core/Utils.h"

#include "ui_ToolBarDialog.h"

#include <QtWidgets/QMenu>

namespace Otter
{

ToolBarDialog::ToolBarDialog(int identifier, QWidget *parent) : QDialog(parent),
	m_definition(ToolBarsManager::getToolBarDefinition(identifier)),
	m_ui(new Ui::ToolBarDialog)
{
	m_ui->setupUi(this);
	m_ui->removeButton->setIcon(Utils::getIcon(QLatin1String("go-previous")));
	m_ui->addButton->setIcon(Utils::getIcon(QLatin1String("go-next")));
	m_ui->moveUpButton->setIcon(Utils::getIcon(QLatin1String("go-up")));
	m_ui->moveDownButton->setIcon(Utils::getIcon(QLatin1String("go-down")));
	m_ui->addEntryButton->setMenu(new QMenu(m_ui->addEntryButton));
	m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(m_definition.canReset);
	m_ui->titleLineEdit->setText(m_definition.title.isEmpty() ? tr("Custom Toolbar") : m_definition.title);
	m_ui->iconSizeSpinBox->setValue(qMax(0, m_definition.iconSize));
	m_ui->maximumButtonSizeSpinBox->setValue(qMax(0, m_definition.maximumButtonSize));

	switch (m_definition.visibility)
	{
		case AlwaysHiddenToolBar:
			m_ui->visibilityComboBox->setCurrentIndex(1);

			break;
		case AutoVisibilityToolBar:
			m_ui->visibilityComboBox->setCurrentIndex(2);

			break;
		default:
			m_ui->visibilityComboBox->setCurrentIndex(0);

			break;
	}

	switch (m_definition.buttonStyle)
	{
		case Qt::ToolButtonFollowStyle:
			m_ui->buttonStyleComboBox->setCurrentIndex(0);

			break;
		case Qt::ToolButtonTextOnly:
			m_ui->buttonStyleComboBox->setCurrentIndex(2);

			break;
		case Qt::ToolButtonTextBesideIcon:
			m_ui->buttonStyleComboBox->setCurrentIndex(3);

			break;
		case Qt::ToolButtonTextUnderIcon:
			m_ui->buttonStyleComboBox->setCurrentIndex(4);

			break;
		default:
			m_ui->buttonStyleComboBox->setCurrentIndex(1);

			break;
	}

	if (!m_definition.bookmarksPath.isEmpty())
	{
		m_ui->optionsHeader->hide();
		m_ui->arrangementWidget->hide();

		adjustSize();
	}

	QStandardItemModel *availableEntriesModel = new QStandardItemModel(this);
	QStandardItem* separatorItem = new QStandardItem(tr("--- separator ---"));
	separatorItem->setData(QLatin1String("separator"), Qt::UserRole);
	separatorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	availableEntriesModel->appendRow(separatorItem);

	QStandardItem* spacerItem = new QStandardItem(tr("--- spacer ---"));
	spacerItem->setData(QLatin1String("spacer"), Qt::UserRole);
	spacerItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	availableEntriesModel->appendRow(spacerItem);

	QStringList widgets;
	widgets << QLatin1String("ClosedWindowsWidget") << QLatin1String("AddressWidget") << QLatin1String("MenuButtonWidget") << QLatin1String("PanelChooserWidget") << QLatin1String("SearchWidget") << QLatin1String("StatusMessageWidget") << QLatin1String("ZoomWidget");

	for (int i = 0; i < widgets.count(); ++i)
	{
		availableEntriesModel->appendRow(createEntry(widgets.at(i)));
	}

	const QVector<ActionDefinition> definitions = ActionsManager::getActionDefinitions();

	for (int i = 0; i < definitions.count(); ++i)
	{
		QStandardItem* item = new QStandardItem(definitions.at(i).icon, (definitions.at(i).description.isEmpty() ? definitions.at(i).text : definitions.at(i).description));
		item->setData(ActionsManager::getActionName(definitions.at(i).identifier) + QLatin1String("Action"), Qt::UserRole);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		availableEntriesModel->appendRow(item);
	}

	m_ui->availableEntriesItemView->setModel(availableEntriesModel);

	QStandardItemModel *currentEntriesModel = new QStandardItemModel(this);

	for (int i = 0; i < m_definition.actions.count(); ++i)
	{
		QStandardItem *item = createEntry(m_definition.actions.at(i).action);
		item->setData(m_definition.actions.at(i).options, (Qt::UserRole + 1));

		currentEntriesModel->appendRow(item);
	}

	m_ui->currentEntriesItemView->setModel(currentEntriesModel);

	m_definition.actions.clear();

	connect(m_ui->addButton, SIGNAL(clicked()), this, SLOT(addEntry()));
	connect(m_ui->removeButton, SIGNAL(clicked()), m_ui->currentEntriesItemView, SLOT(removeRow()));
	connect(m_ui->moveDownButton, SIGNAL(clicked()), m_ui->currentEntriesItemView, SLOT(moveDownRow()));
	connect(m_ui->moveUpButton, SIGNAL(clicked()), m_ui->currentEntriesItemView, SLOT(moveUpRow()));
	connect(m_ui->availableEntriesItemView, SIGNAL(needsActionsUpdate()), this, SLOT(updateActions()));
	connect(m_ui->currentEntriesItemView, SIGNAL(needsActionsUpdate()), this, SLOT(updateActions()));
	connect(m_ui->currentEntriesItemView, SIGNAL(canMoveDownChanged(bool)), m_ui->moveDownButton, SLOT(setEnabled(bool)));
	connect(m_ui->currentEntriesItemView, SIGNAL(canMoveUpChanged(bool)), m_ui->moveUpButton, SLOT(setEnabled(bool)));
	connect(m_ui->availableEntriesFilterLineEdit, SIGNAL(textChanged(QString)), m_ui->availableEntriesItemView, SLOT(setFilter(QString)));
	connect(m_ui->currentEntriesFilterLineEdit, SIGNAL(textChanged(QString)), m_ui->currentEntriesItemView, SLOT(setFilter(QString)));
	connect(m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));
}

ToolBarDialog::~ToolBarDialog()
{
	delete m_ui;
}

void ToolBarDialog::restoreDefaults()
{
	ToolBarsManager::getInstance()->resetToolBar(m_definition.identifier);

	reject();
}

void ToolBarDialog::updateActions()
{
	m_ui->addButton->setEnabled(m_ui->availableEntriesItemView->currentIndex().isValid());
	m_ui->removeButton->setEnabled(m_ui->currentEntriesItemView->currentIndex().isValid() && m_ui->currentEntriesItemView->currentIndex().data(Qt::UserRole).toString() != QLatin1String("MenuBarWidget") && m_ui->currentEntriesItemView->currentIndex().data(Qt::UserRole).toString() != QLatin1String("TabBarWidget"));
}

void ToolBarDialog::changeEvent(QEvent *event)
{
	QDialog::changeEvent(event);

	switch (event->type())
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);

			break;
		default:
			break;
	}
}

QStandardItem* ToolBarDialog::createEntry(const QString &identifier)
{
	QStandardItem *item = new QStandardItem();
	item->setData(identifier, Qt::UserRole);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	if (identifier == QLatin1String("separator"))
	{
		item->setText(tr("--- separator ---"));
	}
	else if (identifier == QLatin1String("spacer"))
	{
		item->setText(tr("--- spacer ---"));
	}
	else if (identifier == QLatin1String("AddressWidget"))
	{
		item->setText(tr("Address Field"));
	}
	else if (identifier == QLatin1String("ClosedWindowsWidget"))
	{
		item->setText(tr("List of Closed Windows and Tabs"));
	}
	else if (identifier == QLatin1String("MenuBarWidget"))
	{
		item->setText(tr("Menu Bar"));
	}
	else if (identifier == QLatin1String("MenuButtonWidget"))
	{
		item->setText(tr("Menu Button"));
	}
	else if (identifier == QLatin1String("PanelChooserWidget"))
	{
		item->setText(tr("Sidebar Panel Chooser"));
	}
	else if (identifier == QLatin1String("SearchWidget"))
	{
		item->setText(tr("Search Field"));
	}
	else if (identifier == QLatin1String("StatusMessageWidget"))
	{
		item->setText(tr("Status Message Field"));
	}
	else if (identifier == QLatin1String("TabBarWidget"))
	{
		item->setText(tr("Tab Bar"));
	}
	else if (identifier == QLatin1String("ZoomWidget"))
	{
		item->setText(tr("Zoom Slider"));
	}
	else if (identifier.startsWith(QLatin1String("bookmarks:")))
	{
		BookmarksItem *bookmark = (identifier.startsWith(QLatin1String("bookmarks:/")) ? BookmarksManager::getModel()->getItem(identifier.mid(11)) : BookmarksManager::getBookmark(identifier.mid(11).toULongLong()));

		if (bookmark)
		{
			item->setText(bookmark->data(BookmarksModel::TitleRole).isValid() ? bookmark->data(BookmarksModel::TitleRole).toString() : tr("(Untitled)"));
			item->setIcon(bookmark->data(Qt::DecorationRole).value<QIcon>());
		}
		else
		{
			item->setText(tr("Invalid Bookmark"));
		}
	}
	else if (identifier.endsWith(QLatin1String("Action")))
	{
		const int actionIdentifier = ActionsManager::getActionIdentifier(identifier.left(identifier.length() - 6));

		if (actionIdentifier < 0)
		{
			item->setText(tr("Invalid Entry"));
		}
		else
		{
			const ActionDefinition definition = ActionsManager::getActionDefinition(actionIdentifier);

			item->setText(definition.description.isEmpty() ? definition.text : definition.description);
			item->setIcon(definition.icon);
		}
	}
	else
	{
		item->setText(tr("Invalid Entry"));
	}

	return item;
}

void ToolBarDialog::addEntry()
{
	QStandardItem *item = m_ui->availableEntriesItemView->getItem(m_ui->availableEntriesItemView->getCurrentRow());

	if (item)
	{
		m_ui->currentEntriesItemView->insertRow(item->clone());
	}
}

ToolBarDefinition ToolBarDialog::getDefinition()
{
	m_definition.title = m_ui->titleLineEdit->text();
	m_definition.iconSize = m_ui->iconSizeSpinBox->value();
	m_definition.maximumButtonSize = m_ui->maximumButtonSizeSpinBox->value();

	switch (m_ui->visibilityComboBox->currentIndex())
	{
		case 1:
			m_definition.visibility = AlwaysHiddenToolBar;

			break;
		case 2:
			m_definition.visibility = AutoVisibilityToolBar;

			break;
		default:
			m_definition.visibility = AlwaysVisibleToolBar;

			break;
	}

	switch (m_ui->buttonStyleComboBox->currentIndex())
	{
		case 0:
			m_definition.buttonStyle = Qt::ToolButtonFollowStyle;

			break;
		case 2:
			m_definition.buttonStyle = Qt::ToolButtonTextOnly;

			break;
		case 3:
			m_definition.buttonStyle = Qt::ToolButtonTextBesideIcon;

			break;
		case 4:
			m_definition.buttonStyle = Qt::ToolButtonTextUnderIcon;

			break;
		default:
			m_definition.buttonStyle = Qt::ToolButtonIconOnly;

			break;
	}

	for (int i = 0; i < m_ui->currentEntriesItemView->model()->rowCount(); ++i)
	{
		ToolBarActionDefinition action;
		action.action = m_ui->currentEntriesItemView->model()->index(i, 0).data(Qt::UserRole).toString();
		action.options = m_ui->currentEntriesItemView->model()->index(i, 0).data(Qt::UserRole + 1).toMap();

		m_definition.actions.append(action);
	}

	return m_definition;
}

}
