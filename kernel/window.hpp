/**
 * @file window.hpp
 * 
 * 表示領域を表すウィンドウ関連.
 */

#pragma once

#include <optional>
#include <vector>
#include "graphics.hpp"
#include "frame_buffer.hpp"

/**
 * @brief 表示領域を表すウィンドウクラス.
 * 
 * タイトルやメニューを持つ一般的なウィンドウだけでなく、マウスカーソルの表示領域なども対象.
 */
class Window {
  public:
    /**
     * @brief Windowと関連付けられたPixelWriter.
     */
    class WindowWriter : public PixelWriter {
      public:
        WindowWriter(Window& window) : window_{window} {
        }

        /**
         * @brief 指定位置に指定色で描画する.
         */
        virtual void Write(Vector2D<int> pos, const PixelColor& c) override {
          window_.Write(pos, c);
        }

        /**
         * Windowの横幅をピクセル単位で返す.
         */
        virtual int Width() const override {
          return window_.Width();
        }

        /**
         * Windowの高さをピクセル単位で返す.
         */
        virtual int Height() const override {
          return window_.Height();
        }
      
      private:
        Window& window_;
    };

    /**
     * @brief 指定された幅・高さの平面描画領域を作成する.
     */
    Window(int width, int height, PixelFormat shadow_format);

    ~Window() = default;

    Window(const Window& rhs) = delete;

    Window& operator =(const Window& rhs) = delete;

    /** 
     * @brief 指定されたFrameBufferおよび位置にこのウィンドウの表示領域を描画する.
     */
    void DrawTo(FrameBuffer& dst, Vector2D<int> position);

    /** 
     * @brief 透過色を設定する.
     */
    void SetTransparentColor(std::optional<PixelColor> c);

    /** 
     * @brief WindowWriterを取得する.
     */
    WindowWriter* Writer();

    /** 
     * @brief 指定された位置のピクセルを返す.
     */
    const PixelColor& At(Vector2D<int> pos) const;

    /** 
     * @brief 指定された位置にピクセルを書き込む.
     */
    void Write(Vector2D<int> pos, PixelColor c);

    /** 
     * @brief 平面描画領域の横幅をピクセル単位で返す.
     */
    int Width() const;

    /** 
     * @brief 平面描画領域の高さをピクセル単位で返す.
     */
    int Height() const;

  private:
    int width_;
    int height_;
    std::vector<std::vector<PixelColor>> data_{};
    WindowWriter writer_{*this};
    std::optional<PixelColor> transparent_color_{std::nullopt};
    FrameBuffer shadow_buffer_{};
};
