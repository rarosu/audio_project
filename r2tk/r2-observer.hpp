/* HEADER
 *
 * File: r2-observer.hpp
 * Created by: Lars Woxberg (Rarosu)
 * Created on: August 29, 2012, 1:58 PM
 *
 * License:
 *   Copyright (C) 2012 Lars Woxberg
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Comments:
 *	Just a simple class for keeping a list of listeners, being able to iterate through them
 *	and exposing a clear interface for adding/removing listeners.
 * Depends on: 
 *
 * Updates:
 * 
 */
#ifndef R2_OBSERVER_HPP
#define R2_OBSERVER_HPP

#include <vector>
#include <algorithm>

namespace r2 {
	/* 
	 * Forward declare
	 */
	template <typename ListenerType>
	class SubscriptionInterface;

	template <typename ListenerType>
	class Subscription;



	/*	Holds the internal listeners, and exposes methods
	 *	for adding/removing known listeners. Should be the
	 *  exposed interface.
	 */
	template <typename ListenerType>
	class SubscriptionInterface {
	public:
		friend class Subscription<ListenerType>;

		/*	Adds a listener to the internal list. If the listener
		 *	already exists in the list, this does nothing.
		 *
		 *	@param p_listener A pointer to the listener
		 */
		void AddListener(ListenerType* p_listener);

		/*	Removes a listener from the internal list. If the listener
		 *	is not in the list, this does nothing.
		 *
		 *	@param p_listener A pointer to the listener
		 */
		void RemoveListener(ListenerType* p_listener);

		/*	Checks whether the listener is in the internal list.
		 *
		 *	@return True if it is in the list. False otherwise.
		 */
		bool IsListening(ListenerType* p_listener) const;
	private:
		std::vector<ListenerType*> m_listeners;
	};



	/*	Wraps around the subscription interface, adding
	 *	the ability to Iterate through the listeners. Should
	 *	be the one constructed inside the notifier.
	 */
	template <typename ListenerType>
	class Subscription {
	public:
		SubscriptionInterface<ListenerType> m_interface;

		std::size_t GetListenerCount() const;
		ListenerType* GetListener(std::size_t index);
	};





	/** IMPLEMENTATION */

	template <typename ListenerType>
	void SubscriptionInterface<ListenerType>::AddListener(ListenerType* p_listener) {
		std::vector<ListenerType*>::iterator it = std::find(m_listeners.begin(), m_listeners.end(), p_listener);
		if (it == m_listeners.end())
			m_listeners.push_back(p_listener);
	}

	template <typename ListenerType>
	void SubscriptionInterface<ListenerType>::RemoveListener(ListenerType* p_listener) {
		std::vector<ListenerType*>::iterator it = std::find(m_listeners.begin(), m_listeners.end(), p_listener);
		if (it != m_listeners.end())
			m_listeners.erase(it);
	}

	template <typename ListenerType>
	bool SubscriptionInterface<ListenerType>::IsListening(ListenerType* p_listener) const {
		return std::find(m_listeners.begin(), m_listeners.end(), p_listener) != m_listeners.end();
	}


	template <typename ListenerType>
	std::size_t Subscription<ListenerType>::GetListenerCount() const {
		return m_interface.m_listeners.size();
	}
	
	template <typename ListenerType>
	ListenerType* Subscription<ListenerType>::GetListener(std::size_t index) {
		return m_interface.m_listeners[index];
	}
}

#endif /* R2_OBSERVER_HPP */