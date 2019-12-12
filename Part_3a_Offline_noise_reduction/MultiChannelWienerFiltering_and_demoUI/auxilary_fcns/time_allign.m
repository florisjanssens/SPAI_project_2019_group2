function [y_sd] = time_allign(y_sd1,y_sd2)
%TIME_ALLIGN Time_allign the two signals using the reference signal y_sd1
%   y_sd1: Reference Signal (does not have to be at index 1)
%   y_sd2: signal to 
    delay = finddelay(y_sd1,y_sd2);
    L1 = length(y_sd1);
    L2 = length(y_sd2);
    y_sd = zeros(L1, 1);
    
    if delay == 0
        if L1 > L2  % y_sd2 is shorter when delayed
            % recompensate the second signal ending 
            y_sd = [y_sd2(1:end); zeros(L1 - L2 -1,1)];
        else
            y_sd = y_sd2(1:L1);
        end
        
    elseif delay > 0 % y_sd2 needs to be shifted left with delay
        if L1 > L2 - delay  % y_sd2 is shorter when delayed
            % recompensate the second signal ending 
            y_sd = [y_sd2(delay:end); zeros((L1 - (L2 - delay) -1),1)];
        else
            y_sd = y_sd2(delay:L1+delay-1);
        end
    
    else
        delay = abs(delay); % y_sd2 needs to be shifted right with delay
        if L1 > L2 + delay  % y_sd2 is shorter even when delayed 
            % Compensate the second signal ending
            y_sd = [zeros(abs(delay),1); y_sd2; zeros((L1 - (L2 + delay)),1)];
        else                % y_sd2 is longer so only zero-pad the beginning
            y_sd = [zeros(abs(delay),1); y_sd2(delay:L1-1)];
        end
    end
    end