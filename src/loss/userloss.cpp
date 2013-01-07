#include "userloss.hpp"

// The constructor does only set values.

cofi::UserLoss::UserLoss(CofiLossFunction& realLoss, bool useOffset) : realLoss(realLoss), useOffset(useOffset) {
}

void cofi::UserLoss::ComputeLossGradient(cofi::WType& w, Real& loss, cofi::WType& grad) {
    if (useOffset) {
        w(cofi::MOVIE_OFFSET_COLUMN, 0) = 1.0;
    }

    realLoss.ComputeLossGradient(w, loss, grad);

    if (useOffset) {
        w(cofi::MOVIE_OFFSET_COLUMN, 0) = 0.0;
        grad(cofi::MOVIE_OFFSET_COLUMN, 0) = 0.0;
    }
}
