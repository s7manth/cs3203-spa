template<typename K, typename V>
OneToManyStore<K, V>::OneToManyStore() = default;

template<typename K, typename V>
OneToManyStore<K, V>::~OneToManyStore() = default;

template<typename K, typename V>
void OneToManyStore<K, V>::insert(K key, V value) {
  this->forward_map_[key].insert(value);
  this->backward_map_[value] = key;

  this->pair_set_.insert(std::make_pair(key, value));
  this->key_set_.insert(key);
  this->value_set_.insert(value);

  this->size++;
}

template<typename K, typename V>
bool OneToManyStore<K, V>::contains(K key, V value) {
  auto iter = this->forward_map_.find(key);
  if (iter == this->forward_map_.end()) return false;
  return iter->second.count(value) > 0;
}

template<typename K, typename V>
bool OneToManyStore<K, V>::containsKey(K key) {
  return this->forward_map_.count(key) > 0;
}

template<typename K, typename V>
bool OneToManyStore<K, V>::containsValue(V value) {
  return this->backward_map_.count(value) > 0;
}

template<typename K, typename V>
std::size_t OneToManyStore<K, V>::length() {
  return this->size;
}

template<typename K, typename V>
std::size_t OneToManyStore<K, V>::numberOfKeys() {
  return this->forward_map_.size();
}

template<typename K, typename V>
std::size_t OneToManyStore<K, V>::numberOfValues() {
  return this->size;
}

template<typename K, typename V>
std::unordered_set<V> OneToManyStore<K, V>::retrieveFromKey(K key) {
  if (!this->containsKey(key)) return {};
  return this->forward_map_[key];
}

template<typename K, typename V>
K OneToManyStore<K, V>::retrieveFromValue(V value) {
  if (!this->containsValue(value)) return K();
  return this->backward_map_[value];
}

template<typename K, typename V>
std::unordered_set<std::pair<K, V>, PairHasherUtil::hash_pair> OneToManyStore<K, V>::retrieveAll() {
  return this->pair_set_;
}

template<typename K, typename V>
std::unordered_set<K> OneToManyStore<K, V>::retrieveAllKeys() {
  return this->key_set_;
}

template<typename K, typename V>
std::unordered_set<V> OneToManyStore<K, V>::retrieveAllValues() {
  return this->value_set_;
}


