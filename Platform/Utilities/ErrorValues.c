#include <stdio.h>
#include <limits.h>

#undef NAMESPACE_DECL_

#define NAMESPACE_DECL_(x) x

    typedef enum
    {
        NAMESPACE_DECL_(kUnknownThreadType) = INT_MIN,
        NAMESPACE_DECL_(kUnknownThread),
        NAMESPACE_DECL_(kInvalidThread),
        NAMESPACE_DECL_(kThreadCreationFailure),
        NAMESPACE_DECL_(kUnknownSemaphore),
        NAMESPACE_DECL_(kUnknownEventBit),
        NAMESPACE_DECL_(kUnknownEvent),
        NAMESPACE_DECL_(kInvalidPriority),
        NAMESPACE_DECL_(kInvalidDelay),
        NAMESPACE_DECL_(kSemaphoreTimeout),
        NAMESPACE_DECL_(kEventTimeout),
        NAMESPACE_DECL_(kBlockInInvalidRegion),
        NAMESPACE_DECL_(kDbgPossibleBlockInRegion),
        NAMESPACE_DECL_(kInvalidPeriod),
        NAMESPACE_DECL_(kAlreadyPeriodic),
        NAMESPACE_DECL_(kNonperiodicSemaphore),
        NAMESPACE_DECL_(kDbgPopUnderflow),

        NAMESPACE_DECL_(kBadIOID),
        NAMESPACE_DECL_(kBadDeviceDescriptor),
        NAMESPACE_DECL_(kOpenFailure),
        NAMESPACE_DECL_(kCloseFailure),
        NAMESPACE_DECL_(kReadFailure),
        NAMESPACE_DECL_(kWriteFailure),
        NAMESPACE_DECL_(kIOCtlFailure),
        NAMESPACE_DECL_(kBadIOTemplateID),

        NAMESPACE_DECL_(kInvalidDeviceFlag),
        NAMESPACE_DECL_(kDeviceTimeout),
        NAMESPACE_DECL_(kDeviceFlagCreationFailure),

        NAMESPACE_DECL_(kMaxCountExceeded),
        NAMESPACE_DECL_(kSemaphoreCreationFailure),
        NAMESPACE_DECL_(kSemaphoreDestructionFailure),
        NAMESPACE_DECL_(kPoolCreationFailure),

        NAMESPACE_DECL_(kInvalidBlockPointer),
        NAMESPACE_DECL_(kInvalidPoolParms),
        NAMESPACE_DECL_(kInvalidPoolID),
        NAMESPACE_DECL_(kErrorPoolNotEmpty),
        NAMESPACE_DECL_(kErrorMallocBlock),

        NAMESPACE_DECL_(kMessageCreationFailure),
        NAMESPACE_DECL_(kInvalidMessageID),
        NAMESPACE_DECL_(kInvalidMessageOwner),
        NAMESPACE_DECL_(kInvalidMessageChannel),
        NAMESPACE_DECL_(kInvalidMessageRecipient),
        NAMESPACE_DECL_(kMessageTimeout),
        NAMESPACE_DECL_(kMessageInQueue),
        NAMESPACE_DECL_(kInvalidTimeout),
        NAMESPACE_DECL_(kInvalidTargetDSP),
        NAMESPACE_DECL_(kIOCreateFailure),
        NAMESPACE_DECL_(kHeapInitialisationFailure),
        NAMESPACE_DECL_(kInvalidHeapID),
        NAMESPACE_DECL_(kNewFailure),
        NAMESPACE_DECL_(kInvalidMarshalledType),
        NAMESPACE_DECL_(kUncaughtException),
        NAMESPACE_DECL_(kAbort),
        NAMESPACE_DECL_(kInvalidMaskBit),
        NAMESPACE_DECL_(kInvalidThreadStatus),
        NAMESPACE_DECL_(kThreadStackOverflow),
        NAMESPACE_DECL_(kMaxIDExceeded),
        NAMESPACE_DECL_(kThreadDestroyedInInvalidRegion),
        NAMESPACE_DECL_(kNotMutexOwner),
        NAMESPACE_DECL_(kMutexNotOwned),
        NAMESPACE_DECL_(kMutexCreationFailure),
        NAMESPACE_DECL_(kMutexDestructionFailure),
        NAMESPACE_DECL_(kMutexSpaceTooSmall),
        NAMESPACE_DECL_(kInvalidMutexID),
        NAMESPACE_DECL_(kInvalidMutexOwner),
        NAMESPACE_DECL_(kAPIUsedfromISR),
        NAMESPACE_DECL_(kMaxHistoryEventExceeded),

        NAMESPACE_DECL_(kIntsAreDisabled)        =-100,
        NAMESPACE_DECL_(kRescheduleIntIsMasked)  =-99,

        NAMESPACE_DECL_(kNoError) = 0,
        NAMESPACE_DECL_(kFirstUserError),
        NAMESPACE_DECL_(kLastUserError) = INT_MAX
    } NAMESPACE_DECL_(SystemError);

main() {

        printf("kUnknownThreadType ,%x %d\n", NAMESPACE_DECL_(kUnknownThreadType) , NAMESPACE_DECL_(kUnknownThreadType) );
        printf("kUnknownThread %x %d\n", NAMESPACE_DECL_(kUnknownThread), NAMESPACE_DECL_(kUnknownThread));
        printf("kInvalidThread %x %d\n", NAMESPACE_DECL_(kInvalidThread), NAMESPACE_DECL_(kInvalidThread));
        printf("kThreadCreationFailure %x %d\n", NAMESPACE_DECL_(kThreadCreationFailure), NAMESPACE_DECL_(kThreadCreationFailure));
        printf("kUnknownSemaphore %x %d\n", NAMESPACE_DECL_(kUnknownSemaphore), NAMESPACE_DECL_(kUnknownSemaphore));
        printf("kUnknownEventBit %x %d\n", NAMESPACE_DECL_(kUnknownEventBit), NAMESPACE_DECL_(kUnknownEventBit));
        printf("kUnknownEvent %x %d\n", NAMESPACE_DECL_(kUnknownEvent), NAMESPACE_DECL_(kUnknownEvent));
        printf("kInvalidPriority %x %d\n", NAMESPACE_DECL_(kInvalidPriority), NAMESPACE_DECL_(kInvalidPriority));
        printf("kInvalidDelay %x %d\n", NAMESPACE_DECL_(kInvalidDelay), NAMESPACE_DECL_(kInvalidDelay));
        printf("kSemaphoreTimeout %x %d\n", NAMESPACE_DECL_(kSemaphoreTimeout), NAMESPACE_DECL_(kSemaphoreTimeout));
        printf("kEventTimeout %x %d\n", NAMESPACE_DECL_(kEventTimeout), NAMESPACE_DECL_(kEventTimeout));
        printf("kBlockInInvalidRegion %x %d\n", NAMESPACE_DECL_(kBlockInInvalidRegion), NAMESPACE_DECL_(kBlockInInvalidRegion));
        printf("kDbgPossibleBlockInRegion %x %d\n", NAMESPACE_DECL_(kDbgPossibleBlockInRegion), NAMESPACE_DECL_(kDbgPossibleBlockInRegion));
        printf("kInvalidPeriod %x %d\n", NAMESPACE_DECL_(kInvalidPeriod), NAMESPACE_DECL_(kInvalidPeriod));
        printf("kAlreadyPeriodic %x %d\n", NAMESPACE_DECL_(kAlreadyPeriodic), NAMESPACE_DECL_(kAlreadyPeriodic));
        printf("kNonperiodicSemaphore %x %d\n", NAMESPACE_DECL_(kNonperiodicSemaphore), NAMESPACE_DECL_(kNonperiodicSemaphore));
        printf("kDbgPopUnderflow %x %d\n", NAMESPACE_DECL_(kDbgPopUnderflow), NAMESPACE_DECL_(kDbgPopUnderflow));
        printf("kBadIOID %x %d\n", NAMESPACE_DECL_(kBadIOID), NAMESPACE_DECL_(kBadIOID));
        printf("kBadDeviceDescriptor %x %d\n", NAMESPACE_DECL_(kBadDeviceDescriptor), NAMESPACE_DECL_(kBadDeviceDescriptor));
        printf("kOpenFailure %x %d\n", NAMESPACE_DECL_(kOpenFailure), NAMESPACE_DECL_(kOpenFailure));
        printf("kCloseFailure %x %d\n", NAMESPACE_DECL_(kCloseFailure), NAMESPACE_DECL_(kCloseFailure));
        printf("kReadFailure %x %d\n", NAMESPACE_DECL_(kReadFailure), NAMESPACE_DECL_(kReadFailure));
        printf("kWriteFailure %x %d\n", NAMESPACE_DECL_(kWriteFailure), NAMESPACE_DECL_(kWriteFailure));
        printf("kIOCtlFailure %x %d\n", NAMESPACE_DECL_(kIOCtlFailure), NAMESPACE_DECL_(kIOCtlFailure));
        printf("kBadIOTemplateID %x %d\n", NAMESPACE_DECL_(kBadIOTemplateID), NAMESPACE_DECL_(kBadIOTemplateID));
        printf("kInvalidDeviceFlag %x %d\n", NAMESPACE_DECL_(kInvalidDeviceFlag), NAMESPACE_DECL_(kInvalidDeviceFlag));
        printf("kDeviceTimeout %x %d\n", NAMESPACE_DECL_(kDeviceTimeout), NAMESPACE_DECL_(kDeviceTimeout));
        printf("kDeviceFlagCreationFailure %x %d\n", NAMESPACE_DECL_(kDeviceFlagCreationFailure), NAMESPACE_DECL_(kDeviceFlagCreationFailure));
        printf("kMaxCountExceeded %x %d\n", NAMESPACE_DECL_(kMaxCountExceeded), NAMESPACE_DECL_(kMaxCountExceeded));
        printf("kSemaphoreCreationFailure %x %d\n", NAMESPACE_DECL_(kSemaphoreCreationFailure), NAMESPACE_DECL_(kSemaphoreCreationFailure));
        printf("kSemaphoreDestructionFailure %x %d\n", NAMESPACE_DECL_(kSemaphoreDestructionFailure), NAMESPACE_DECL_(kSemaphoreDestructionFailure));
        printf("kPoolCreationFailure %x %d\n", NAMESPACE_DECL_(kPoolCreationFailure), NAMESPACE_DECL_(kPoolCreationFailure));
        printf("kInvalidBlockPointer %x %d\n", NAMESPACE_DECL_(kInvalidBlockPointer), NAMESPACE_DECL_(kInvalidBlockPointer));
        printf("kInvalidPoolParms %x %d\n", NAMESPACE_DECL_(kInvalidPoolParms), NAMESPACE_DECL_(kInvalidPoolParms));
        printf("kInvalidPoolID %x %d\n", NAMESPACE_DECL_(kInvalidPoolID), NAMESPACE_DECL_(kInvalidPoolID));
        printf("kErrorPoolNotEmpty %x %d\n", NAMESPACE_DECL_(kErrorPoolNotEmpty), NAMESPACE_DECL_(kErrorPoolNotEmpty));
        printf("kErrorMallocBlock %x %d\n", NAMESPACE_DECL_(kErrorMallocBlock), NAMESPACE_DECL_(kErrorMallocBlock));
        printf("kMessageCreationFailure %x %d\n", NAMESPACE_DECL_(kMessageCreationFailure), NAMESPACE_DECL_(kMessageCreationFailure));
        printf("kInvalidMessageID %x %d\n", NAMESPACE_DECL_(kInvalidMessageID), NAMESPACE_DECL_(kInvalidMessageID));
        printf("kInvalidMessageOwner %x %d\n", NAMESPACE_DECL_(kInvalidMessageOwner), NAMESPACE_DECL_(kInvalidMessageOwner));
        printf("kInvalidMessageChannel %x %d\n", NAMESPACE_DECL_(kInvalidMessageChannel), NAMESPACE_DECL_(kInvalidMessageChannel));
        printf("kInvalidMessageRecipient %x %d\n", NAMESPACE_DECL_(kInvalidMessageRecipient), NAMESPACE_DECL_(kInvalidMessageRecipient));
        printf("kMessageTimeout %x %d\n", NAMESPACE_DECL_(kMessageTimeout), NAMESPACE_DECL_(kMessageTimeout));
        printf("kMessageInQueue %x %d\n", NAMESPACE_DECL_(kMessageInQueue), NAMESPACE_DECL_(kMessageInQueue));
        printf("kInvalidTimeout %x %d\n", NAMESPACE_DECL_(kInvalidTimeout), NAMESPACE_DECL_(kInvalidTimeout));
        printf("kInvalidTargetDSP %x %d\n", NAMESPACE_DECL_(kInvalidTargetDSP), NAMESPACE_DECL_(kInvalidTargetDSP));
        printf("kIOCreateFailure %x %d\n", NAMESPACE_DECL_(kIOCreateFailure), NAMESPACE_DECL_(kIOCreateFailure));
        printf("kHeapInitialisationFailure %x %d\n", NAMESPACE_DECL_(kHeapInitialisationFailure), NAMESPACE_DECL_(kHeapInitialisationFailure));
        printf("kInvalidHeapID %x %d\n", NAMESPACE_DECL_(kInvalidHeapID), NAMESPACE_DECL_(kInvalidHeapID));
        printf("kNewFailure %x %d\n", NAMESPACE_DECL_(kNewFailure), NAMESPACE_DECL_(kNewFailure));
        printf("kInvalidMarshalledType %x %d\n", NAMESPACE_DECL_(kInvalidMarshalledType), NAMESPACE_DECL_(kInvalidMarshalledType));
        printf("kUncaughtException %x %d\n", NAMESPACE_DECL_(kUncaughtException), NAMESPACE_DECL_(kUncaughtException));
        printf("kAbort %x %d\n", NAMESPACE_DECL_(kAbort), NAMESPACE_DECL_(kAbort));
        printf("kInvalidMaskBit %x %d\n", NAMESPACE_DECL_(kInvalidMaskBit), NAMESPACE_DECL_(kInvalidMaskBit));
        printf("kInvalidThreadStatus %x %d\n", NAMESPACE_DECL_(kInvalidThreadStatus), NAMESPACE_DECL_(kInvalidThreadStatus));
        printf("kThreadStackOverflow %x %d\n", NAMESPACE_DECL_(kThreadStackOverflow), NAMESPACE_DECL_(kThreadStackOverflow));
        printf("kMaxIDExceeded %x %d\n", NAMESPACE_DECL_(kMaxIDExceeded), NAMESPACE_DECL_(kMaxIDExceeded));
        printf("kThreadDestroyedInInvalidRegion %x %d\n", NAMESPACE_DECL_(kThreadDestroyedInInvalidRegion), NAMESPACE_DECL_(kThreadDestroyedInInvalidRegion));
        printf("kNotMutexOwner %x %d\n", NAMESPACE_DECL_(kNotMutexOwner), NAMESPACE_DECL_(kNotMutexOwner));
        printf("kMutexNotOwned %x %d\n", NAMESPACE_DECL_(kMutexNotOwned), NAMESPACE_DECL_(kMutexNotOwned));
        printf("kMutexCreationFailure %x %d\n", NAMESPACE_DECL_(kMutexCreationFailure), NAMESPACE_DECL_(kMutexCreationFailure));
        printf("kMutexDestructionFailure %x %d\n", NAMESPACE_DECL_(kMutexDestructionFailure), NAMESPACE_DECL_(kMutexDestructionFailure));
        printf("kMutexSpaceTooSmall %x %d\n", NAMESPACE_DECL_(kMutexSpaceTooSmall), NAMESPACE_DECL_(kMutexSpaceTooSmall));
        printf("kInvalidMutexID %x %d\n", NAMESPACE_DECL_(kInvalidMutexID), NAMESPACE_DECL_(kInvalidMutexID));
        printf("kInvalidMutexOwner %x %d\n", NAMESPACE_DECL_(kInvalidMutexOwner), NAMESPACE_DECL_(kInvalidMutexOwner));
        printf("kAPIUsedfromISR %x %d\n", NAMESPACE_DECL_(kAPIUsedfromISR), NAMESPACE_DECL_(kAPIUsedfromISR));
        printf("kMaxHistoryEventExceeded %x %d\n", NAMESPACE_DECL_(kMaxHistoryEventExceeded), NAMESPACE_DECL_(kMaxHistoryEventExceeded));
        printf("kIntsAreDisabled %x %d\n", NAMESPACE_DECL_(kIntsAreDisabled)        , NAMESPACE_DECL_(kIntsAreDisabled)        );
        printf("kRescheduleIntIsMasked %x %d\n", NAMESPACE_DECL_(kRescheduleIntIsMasked)  , NAMESPACE_DECL_(kRescheduleIntIsMasked)  );
        printf("kNoError %x %d\n", NAMESPACE_DECL_(kNoError) , NAMESPACE_DECL_(kNoError) );
        printf("kFirstUserError %x %d\n", NAMESPACE_DECL_(kFirstUserError), NAMESPACE_DECL_(kFirstUserError));
        printf("kLastUserError %x %d\n", NAMESPACE_DECL_(kLastUserError) , NAMESPACE_DECL_(kLastUserError) );

}

