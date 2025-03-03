#include "engine/MainView.h"

#include "imgui.h"

MainView::MainView()
{
    SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);
}

MainView::~MainView()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void MainView::update()
{

}

void MainView::render()
{
    ImGui::Begin("Game Viewport");

    ImGui::Text("Game Rendering Will go here");
    // I wanna try open gl rendering here thats why I branched the git repo

    ImGui::End();
}
