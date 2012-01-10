#include <cinder/app/AppBasic.h>
#include <cinder/Camera.h>
#include <cinder/Arcball.h>

#include <Eigen/Core>

using namespace ci;
using namespace ci::app;

using Eigen::ArrayX3d;
using Eigen::ArrayXd;

struct ParticleSystem {
    ArrayX3d pos, vel;
    ArrayXd life;

    ParticleSystem() {}

    explicit ParticleSystem(int size)
    {
        pos = ArrayX3d::Random(size, 3);
        vel = ArrayX3d::Random(size, 3);
        life = 1.0 + 5.0 * ArrayXd::Random(size);
    }

    void advance(double dt)
    {
        // trust the compiler to optimize this
        pos += vel * (dt * (life>0.0).cast<double>()).replicate<1,3>();
        life -= dt;
    }
};

class Sluk : public AppBasic {
    ParticleSystem m_partsys;
    Timer m_timer;
    CameraPersp m_cam;
    Arcball m_arcball;

    void setup() {
        m_partsys = ParticleSystem(10000);
        m_cam.setPerspective( 90.0f, getWindowAspectRatio(), 0.01f, 50.0f );
        m_cam.lookAt(Vec3f(0,0,10), Vec3f(0,0,0));
    }
    void update() {
        m_timer.stop();
        float dt = m_timer.getSeconds();
        m_timer.start();

        m_partsys.advance(dt);
    }
    void draw() {
        gl::setMatrices( m_cam );
        gl::clear(Color(1,1,1), true);
        gl::color(0,0,0);

        gl::rotate( m_arcball.getQuat() );
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer( 3, GL_DOUBLE, 0, m_partsys.pos.data() );
        glDrawArrays(GL_POINTS, 0, m_partsys.pos.rows());
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    void resize(ResizeEvent e) {
        m_arcball.setCenter( getWindowCenter() );
        m_arcball.setRadius( getWindowSize().y / 2 );
        m_cam.setAspectRatio(e.getAspectRatio());
    }

    void mouseDown( MouseEvent e )
    {
        m_arcball.mouseDown( e.getPos() );
    }

    void mouseDrag( MouseEvent e )
    {
        m_arcball.mouseDrag( e.getPos() );
    }

};

CINDER_APP_BASIC( Sluk, RendererGl )
