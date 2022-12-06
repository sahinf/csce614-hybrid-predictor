# CSCE 614 Advanced Computer Architecture hybrid branch predictor

A hybrid branch predictor combining a Piecewise Linear predictor with a TAGE predictor

# Results

## SOLO Piecewise Linear
Average MPKI = 7.040

## SOLO TAGE
Average MPKI = 7.143

## Saturated Counter
Average MPKI results
Saturated Counter to switch between the 2

### Average direction MPKI
| sat_range | Average MPKI |
| --------- | ------------ |
| 16        | 7.146        |
| 32        | 7.148        |
| 64        | 7.146        |
| 128       | 7.099        |
| 256       | 7.121        |
| 512       | 7.169        |
| 1024      | 7.173        |

### Conclusion
This is not a good strategy, but a hybrid saturation has less MPKI than solo TAGE but more MPKI than solo piecewise 

