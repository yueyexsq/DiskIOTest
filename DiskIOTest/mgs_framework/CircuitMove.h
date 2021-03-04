#pragma once


//take a circuitous route
//  rate matrix of x-y no z
//  |  cos  sin  0   0  |
//  | -sin  cos  0   0  |
//  |   0    0   1   0  |
//  |   0    0   0   1  |

class CircuitMove
{
public:
    CircuitMove():m_sx(0.f),m_sy(0.f),m_sz(0.f),m_rx(0.f),m_ry(0.f),m_rz(0.f),m_speed(0),m_totaltime(0),m_movetime(-1),m_radius(0){}
    ~CircuitMove(){}

    void StartMove(float sx,float sy,float sz,float rx,float ry,float rz,float speed,unsigned totaltime)
    {
        m_sx=sx-rx;m_sy=sy-ry;
        m_rx=rx;m_ry=ry;
        m_speed=speed;
        m_totaltime=totaltime;
        m_movetime=0;
        m_radius=sqrtf(m_sx*m_sx+m_sy*m_sy);
        m_curx=sx;m_cury=sy;
        m_sz = sz;
        m_rz = rz;
    }

    void Update(const unsigned diff)
    {
        if (m_movetime>m_totaltime)
        {
            return;
        }
        m_movetime+=diff;
        if (m_movetime>m_totaltime)
        {
            m_movetime=m_totaltime;
        }

        float rp = m_speed*m_movetime/m_radius;
        float rsin = sinf(rp);
        float rcos = cosf(rp);
        m_curx = m_sx*rcos - m_sy*rsin + m_rx;
        m_cury = m_sx*rsin + m_sy*rcos + m_ry;
        m_curz = m_sz;
    }

    bool isComplete() const
    {
        return m_movetime == m_totaltime ? true : false;
    }

    bool GetCurPos(float& curx,float& cury,float &curz)
    {
        if (m_movetime==-1)
            return false;
        curx=m_curx;cury=m_cury;curz = m_curz;
        return true;
    }

    void Clear() {m_movetime=-1;}
    bool IsValid() {return m_movetime!=-1;}
protected:
    float m_sx;
    float m_sy;
    float m_sz;
    float m_rx;
    float m_ry;
    float m_rz;
    float m_speed;
    float m_radius;
    float m_curx;
    float m_cury;
    float m_curz;
    unsigned m_totaltime;
    unsigned m_movetime;
};

