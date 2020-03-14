#include <iostream>
#include <random>

double GenRandom() {
	static std::mt19937_64 generator;
	std::uniform_real_distribution<double> distr(0, 1);
	return distr(generator);
}

template <typename Key, typename Priority>
class Treap {
public:
	Treap() = default;

	~Treap() {
		delete root_;
	}

	void Insert(const Key& key, const Priority& priority) {
		if (!Find(key)) {
			auto left_and_right = Split(root_, key);
			left_and_right.first = Merge(left_and_right.first, new TreapNode(key, priority));
			root_ = Merge(left_and_right.first, left_and_right.second);
		}
	}

	void Erase(const Key& key) {
		if (Find(key)) {
			auto left_and_right_1 = Split(root_, key);
			auto left_and_right_2 = Split(left_and_right_1.second, key + 1);
			root_ = Merge(left_and_right_1.first, left_and_right_2.second);
		}
	}

	bool Find(const Key& key) const {
		return Find(key, root_);
	}

	size_t Size() const {
		return GetSize(root_);
	}

	Key Next(const Key& key) const {
		auto left_and_right = Split(root_, key + 1);
		if (left_and_right.second) {
			auto answer = (left_and_right.second)->key;
			auto iter = left_and_right.second;
			while (iter->left) {
				answer = (iter->left)->key;
				iter = iter->left;
			}
			root_ = Merge(left_and_right.first, left_and_right.second);
			return answer;
		}
		root_ = Merge(left_and_right.first, left_and_right.second);
		return -1;
	}

	Key Prev(const Key& key) const {
		auto left_and_right = Split(root_, key);
		if (left_and_right.first) {
			auto answer = (left_and_right.first)->key;
			auto iter = left_and_right.first;
			while (iter->right) {
				answer = (iter->right)->key;
				iter = iter->right;
			}
			root_ = Merge(left_and_right.first, left_and_right.second);
			return answer;
		}
		root_ = Merge(left_and_right.first, left_and_right.second);
		return -1;
	}

	Key Kth(size_t index) const {
		if (GetSize(root_) && index > root_->size || !GetSize(root_)) {
			return -1;
		}
		auto iter = root_;
		while (index) {
			auto lsize = GetSize(iter->left);
			if (lsize == index - 1) {
				return iter->key;
			} else if (lsize > index - 1) {
				iter = iter->left;
			} else if (lsize < index - 1) {
				iter = iter->right;
				index -= lsize + 1;
			}
		}
	}
private:
	struct TreapNode {
		Key key;
		Priority priority;
		size_t size = 1;
		TreapNode* left = nullptr;
		TreapNode* right = nullptr;
		TreapNode* parent = nullptr;

		TreapNode(const Key& key, const Priority& priority) :
			key(key), priority(priority) {}

		~TreapNode() {
			delete left;
			delete right;
		}
	};

	static size_t GetSize(TreapNode* node) {
		return node ? node->size : 0;
	};

	static void UpdateSize(TreapNode* node) {
		if (!node) {
			return;
		}
		node->size = GetSize(node->left) + GetSize(node->right) + 1;
	}

	static void SetLeft(TreapNode* root, TreapNode* new_left) {
		if (root) {
			root->left = new_left;
			UpdateSize(root);
		}
		if (new_left) {
			new_left->parent = root;
		}
	}

	static void SetRight(TreapNode* root, TreapNode* new_right) {
		if (root) {
			root->right = new_right;
			UpdateSize(root);
		}
		if (new_right) {
			new_right->parent = root;
		}
	}

	static TreapNode* Merge(TreapNode* left_treap, TreapNode* right_treap) {
		if (!left_treap) {
			return right_treap;
		}
		if (!right_treap) {
			return left_treap;
		}
		if (left_treap->priority < right_treap->priority) {
			SetLeft(right_treap, Merge(left_treap, right_treap->left));
			return right_treap;
		} else {
			SetRight(left_treap, Merge(left_treap->right, right_treap));
			return left_treap;
		}
	}

	static std::pair<TreapNode*, TreapNode*> Split(TreapNode* treap, const Key& key) {
		if (!treap) {
			return { nullptr, nullptr };
		}
		if (treap->key < key) {
			auto left_and_right = Split(treap->right, key);
			SetRight(treap, left_and_right.first);
			SetLeft(nullptr, left_and_right.second);
			return { treap, left_and_right.second };
		} else {
			auto left_and_right = Split(treap->left, key);
			SetLeft(treap, left_and_right.second);
			SetRight(nullptr, left_and_right.first);
			return { left_and_right.first, treap };
		}
	}

	bool Find(const Key& key, const TreapNode* node) const {
		if (!node) {
			return false;
		}
		if (key == node->key) {
			return true;
		} else if (key < node->key) {
			return Find(key, node->left);
		} else {
			return Find(key, node->right);
		}
	}

	mutable TreapNode* root_ = nullptr;
};
