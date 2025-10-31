#include "demo_mvc.hpp"


DemoApplication::DemoApplication(DemoViewModel& view_model, GLFWwindow *window):
    m_view_model(view_model),
    m_window(window){

        m_view_model.setCameraTarget(m_view_model.getPlanetCenterVec(selected_planet_index));
        
        UpdateRingsItems();
    }