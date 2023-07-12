/**
 * @file layer.hpp
 * 
 * 重ね合わせ処理など.
 */

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "graphics.hpp"
#include "window.hpp"

/**
 * @brief Layerは1つの層を表す.
 * 
 * 現状は1つのウィンドウのみ保持できる.
 * 将来的には複数を保持しうる.
 */
class Layer {
  public:
    Layer(unsigned int id = 0);

    unsigned int ID() const;

    Layer& SetWindow(const std::shared_ptr<Window>& window);

    std::shared_ptr<Window> GetWindow() const;

    /** @brief 位置情報を指定された絶対座標に更新する. 再描画しない. */
    Layer& Move(Vector2D<int> pos);
 
    /** @brief 位置情報を指定された相対座標に更新する. 再描画しない. */
    Layer& MoveRelative(Vector2D<int> pos_diff);

    /** @brief 指定されたwriterに現在のウィンドウの内容を描画する. */
    void DrawTo(PixelWriter& writer) const;

  private:
    unsigned int id_;
    Vector2D<int> pos_;
    std::shared_ptr<Window> window_;
};

/**
 * @brief LayerManagerは複数のレイヤーを管理する.
 */
class LayerManager {
  public:
    /** @brief 描画先を設定する. */
    void SetWriter(PixelWriter* writer);

    /**
     * @brief 新しいレイヤーを生成してその参照を返す.
     * 
     * 生成したレイヤーは内部のコンテナに保持する.
     */
    Layer& NewLayer();

    void Draw() const;

    /** @brief idで指定されたレイヤーの位置情報を、指定された絶対座標に更新する. 再描画しない. */
    void Move(unsigned int id, Vector2D<int> new_position);

    /** @brief idで指定されたレイヤーの位置情報を、指定された相対座標に更新する. 再描画しない. */
    void MoveRelative(unsigned int id, Vector2D<int> poss_diff);

    /**
     * @brief レイヤーのZ座標上の位置を移動する.
     * 
     * new_zが負の値の場合は非表示に、0以上の場合はその位置に移動する.
     * 現在のレイヤー数以上の数値が指定された場合は最前面に移動する.
     */
    void UpDown(unsigned int id, int new_z);

    /**
     * @brief レイヤーを非表示にする.
     */
    void Hide(unsigned int id);

  private:
    PixelWriter* writer_{nullptr};
    std::vector<std::unique_ptr<Layer>> layers_{};
    std::vector<Layer*> layer_stack_{};
    unsigned int latest_id_{0};

    Layer* FindLayer(unsigned int id);
};

extern LayerManager* layer_manager;
