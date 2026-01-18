#include "DX11.Framework.h"

namespace framework {
	namespace core {
		namespace VARS {
			namespace time {
				FPSLimiter* fps_limiter;
				TimerHighResolution timer_program;
			}

			void InitTime() {
				VARS::time::fps_limiter = new FPSLimiter;
				VARS::time::timer_program.begin();
			}
			void TerminateTime() {
				delete VARS::time::fps_limiter;
				VARS::time::fps_limiter = nullptr;
			}

		}
	}
	namespace time {

		using namespace framework::core::VARS::time;

		void  SetTimeProgram() {
			timer_program.begin();
		}

		float GetTimeProgram() {
			timer_program.end();
			return timer_program.getTimeElapsed();
		}

		float GetDeltaTime() {
			return fps_limiter->GetDeltaTime();
		}

		void  SetFPS(const unsigned int& fps) {
			fps_limiter->SetFPS((float)fps);
		}

		float GetFPS() {
			return fps_limiter->GetFPS();
		}
	}
}