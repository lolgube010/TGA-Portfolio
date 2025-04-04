#pragma once
#include <d3d11.h>
#include <wrl/client.h>  // For Microsoft::WRL::ComPtr

class DX11Timer {
public:
    DX11Timer() = default; // Default constructor

    bool Init(ID3D11Device* device, ID3D11DeviceContext* context) {
        if (!device || !context) return false; // Ensure valid device/context

        m_device = device;
        m_context = context;

        return InitializeQueries();
    }

    void BeginFrame() {
        if (!m_initialized) return;
        m_context->Begin(m_disjointQuery);
        m_context->End(m_startQuery);
    }

    void EndFrame() {
        if (!m_initialized) return;
        m_context->End(m_endQuery);
        m_context->End(m_disjointQuery);
    }

    void CalculateFrameTime() {
        if (!m_initialized) return;

        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
        UINT64 startTime = 0, endTime = 0;

        while (m_context->GetData(m_disjointQuery, &disjointData, sizeof(disjointData), 0) == S_FALSE);
        while (m_context->GetData(m_startQuery, &startTime, sizeof(startTime), 0) == S_FALSE);
        while (m_context->GetData(m_endQuery, &endTime, sizeof(endTime), 0) == S_FALSE);

        if (!disjointData.Disjoint && disjointData.Frequency > 0) {
            m_frameTime = (endTime - startTime) / static_cast<double>(disjointData.Frequency);
            m_fps = 1.0 / m_frameTime;
        }
    }

    float GetDeltaTime() const { return static_cast<float>(m_frameTime); }
    float GetFPS() const { return static_cast<float>(m_fps); }

private:
    bool InitializeQueries() {
        D3D11_QUERY_DESC desc = {};

        // Disjoint query (for checking validity)
        desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        if (FAILED(m_device->CreateQuery(&desc, &m_disjointQuery))) return false;

        // Start & end timestamp queries
        desc.Query = D3D11_QUERY_TIMESTAMP;
        if (FAILED(m_device->CreateQuery(&desc, &m_startQuery))) return false;
        if (FAILED(m_device->CreateQuery(&desc, &m_endQuery))) return false;

        m_initialized = true;
        return true;
    }

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;

    ID3D11Query* m_disjointQuery = nullptr;
    ID3D11Query* m_startQuery = nullptr;
    ID3D11Query* m_endQuery = nullptr;

    double m_frameTime = 0.0;
    double m_fps = 0.0;
    bool m_initialized = false;
};
