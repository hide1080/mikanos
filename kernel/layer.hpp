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

    /** @brief ウィンドウを設定する．既存のウィンドウは上書きされる． */
    Layer& SetWindow(const std::shared_ptr<Window>& window);

    /** @brief ウィンドウを返す． */
    std::shared_ptr<Window> GetWindow() const;

    /** @brief レイヤーの原点座標を返す． */
    Vector2D<int> GetPosition() const;

    /** @brief 位置情報を指定された絶対座標に更新する. 再描画しない. */
    Layer& Move(Vector2D<int> pos);
 
    /** @brief 位置情報を指定された相対座標に更新する. 再描画しない. */
    Layer& MoveRelative(Vector2D<int> pos_diff);

    /** @brief 指定された描画先にウィンドウの内容を描画する. */
    void DrawTo(FrameBuffer& screen, const Rectangle<int>& area) const;

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
    void SetWriter(FrameBuffer* screen);

    /**
     * @brief 新しいレイヤーを生成してその参照を返す.
     * 
     * 生成したレイヤーは内部のコンテナに保持する.
     */
    Layer& NewLayer();

    /** @brief 現在表示状態にあるレイヤーの指定された領域を描画する． */
    void Draw(const Rectangle<int>& area) const;

    /** @brief 指定されたレイヤーが持つウィンドウの領域を描画する． */
    void Draw(unsigned int id) const;

    /** @brief idで指定されたレイヤーの位置情報を、指定された絶対座標に更新する. 再描画する. */
    void Move(unsigned int id, Vector2D<int> new_pos);

    /** @brief idで指定されたレイヤーの位置情報を、指定された相対座標に更新する. 再描画する. */
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

    /**
     * @brief 指定された座標にウィンドウを持つ最も上部のレイヤーを探す.
     */
    Layer* FindLayerByPosition(Vector2D<int> pos,
                               unsigned int exclude_id) const;

  private:
    FrameBuffer* screen_{nullptr};
    mutable FrameBuffer back_buffer_{};
    std::vector<std::unique_ptr<Layer>> layers_{};
    std::vector<Layer*> layer_stack_{};
    unsigned int latest_id_{0};

    Layer* FindLayer(unsigned int id);
};

extern LayerManager* layer_manager;
