/***************************************************************************
 *   (C) 2005 Ejner Borgbjerg <ejner@users.sourceforge.net>                *
 *   (C) 2006 William Hoggarth <whoggarth@users.sourceforge.net>           *
 *   (C) 2007-2009 Michal Rudolf <mrudolf@kdewebdev.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "database.h"
#include "filter.h"
#include "filtersearch.h"
#include <QtDebug>

using namespace chessx;

#if defined(_MSC_VER) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif // _MSC_VER

FilterX::FilterX(Database* database) : QThread()
{
    m_database = database;
    m_count = m_database->count();
    m_vector = new QVector<FilterX::value_type>(m_count, 1);
    m_gamesSearched = 0;
    m_searchTime = 0;
    currentSearchOperator = NullOperator;
    m_break = false;
}

FilterX::~FilterX()
{
    cancel();
    delete currentSearch;
    delete m_vector;
}

FilterX::FilterX(FilterX const& rhs) : QThread()
{
    m_vector = nullptr;
    *this = rhs;
}

FilterX& FilterX::operator= (FilterX const& rhs)
{
    if (this!=&rhs)
    {
        m_database = rhs.m_database;
        m_count = rhs.m_count;
        delete m_vector;
        m_vector = new QVector<FilterX::value_type>(*rhs.m_vector);
        m_gamesSearched = 0;
        m_searchTime = 0;
        currentSearch = nullptr;
        currentSearchOperator = NullOperator;
        m_break = false;
        m_lock = nullptr;
    }
    return *this;
}

const Database* FilterX::database() const
{
    return m_database;
}

void FilterX::lock(FilterX* locked)
{
   m_lock = locked;
}

Database* FilterX::database()
{
    return m_database;
}

void FilterX::set(GameId game, FilterX::value_type value)
{
    if((game >= size()) || (gamePosition(game) == value))
    {
        return;
    }
    if(value && !contains(game))
    {
        ++m_count;
    }
    else if(!value && contains(game))
    {
        --m_count;
    }

    (*m_vector)[game] = value;
}

void FilterX::setAll(FilterX::value_type value)
{
    cancel();
    m_vector->fill(value);
    m_count = value ? size() : 0;
}

bool FilterX::contains(GameId game) const
{
    if(static_cast<int>(game) < m_vector->count())
    {
        return (m_vector->at(game) != 0);
    }
    return false;
}

FilterX::value_type FilterX::gamePosition(GameId game) const
{
    return m_vector->at(static_cast<int>(game));
}

unsigned int FilterX::size() const
{
    return static_cast<unsigned int>(m_vector->size());
}

void FilterX::resize(unsigned int newsize, bool includeNew)
{
    for(GameId i = static_cast<GameId>(newsize); i < size(); ++i)   // Decrease count by number of removed games
    {
        if(contains(i))
        {
            --m_count;
        }
    }
    unsigned int oldsize = size();
    m_vector->resize(newsize);
    // Set new (uninitialized games) to 'includeNew' value.
    for(unsigned int i = oldsize; i < newsize; ++i)
    {
        (*m_vector)[i] = includeNew;
    }
    if(includeNew)
    {
        m_count += newsize - oldsize;
    }
}

void FilterX::invert()
{
    cancel();
    m_count = size() - m_count;
    for(int i = 0, sz = static_cast<int>(size()); i < sz; ++i)
    {
        if(m_vector->at(i))
        {
            (*m_vector)[i] = 0;
        }
        else
        {
            (*m_vector)[i] = 1;
        }
    }
}

void FilterX::runSingleSearch(Search* s, FilterOperator op)
{
    connect(s, SIGNAL(prepareUpdate(int)), this, SIGNAL(searchProgress(int)));
    s->Prepare(m_break);
    switch (op)
    {
    case FilterOperator::NullOperator:
        for(int searchIndex = 0, sz = static_cast<int>(size()); searchIndex < sz; ++searchIndex)
        {
            if (m_break) break;
            set(searchIndex, s->matches(searchIndex));
            if (searchIndex % 1024 == 0) emit searchProgress(searchIndex*100/size());
        }
        break;
    case FilterOperator::And:
        for(int searchIndex = 0, sz = static_cast<int>(size()); searchIndex < sz; ++searchIndex)
        {
            if (m_break) break;
            if (contains(searchIndex))
            {
                int n = s->matches(searchIndex);
                if (n!=1) // Better search result or and does not apply
                {
                    set(searchIndex, n);
                }
            }
            if (searchIndex % 1024 == 0) emit searchProgress(searchIndex*100/size());
        }
        break;
    case FilterOperator::Or:
        for(int searchIndex = 0, sz = static_cast<int>(size()); searchIndex < sz; ++searchIndex)
        {
            if (m_break) break;
            if (!contains(searchIndex))
            {
                int n = s->matches(searchIndex);
                if (n)
                {
                    set(searchIndex, n);
                }
            }
            if (searchIndex % 1024 == 0) emit searchProgress(searchIndex*100/size());
        }
        break;
    case FilterOperator::Remove:
        for(int searchIndex = 0, sz = static_cast<int>(size()); searchIndex < sz; ++searchIndex)
        {
            if (m_break) break;
            if (contains(searchIndex) && s->matches(searchIndex))
            {
                set(searchIndex, 0);
            }
            if (searchIndex % 1024 == 0) emit searchProgress(searchIndex*100/size());
        }
        break;
    default:
        break;
    }
}

void FilterX::run()
{
    Search* s = currentSearch;
    FilterOperator op = currentSearchOperator;
    while(s)
    {
        runSingleSearch(s, op);
        op = s->searchOperator();
        s = s->nextSearch();
    }
    delete currentSearch;
    emit searchProgress(100);
    if (!m_break)
    {
        emit searchFinished();
    }
}


void FilterX::cancel()
{
    if (isRunning())
    {
        m_break = true;
        wait();
    }
    if (m_lock && m_lock->isRunning())
    {
        m_lock->cancel();
        m_lock->wait();
    }
}

void FilterX::executeSearch(Search* search, FilterOperator searchOperator)
{
    if (searchOperator == FilterOperator::NullOperator)
    {
        setAll(0); // ??
    }
    m_break = false;
    currentSearch = search;
    currentSearchOperator = searchOperator;
    start();
}
